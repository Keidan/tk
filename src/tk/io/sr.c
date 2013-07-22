#include <tk/io/sr.h>
#include <tk/sys/log.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#define SR_MAGIC     0x532141
#define SCAST(sr)    ((struct sr_s*)sr)
#define IS_VALID(sr) (sr && SCAST(sr)->magic == SR_MAGIC)



struct sr_s {
    int magic;
    int fd;
    struct sr_cfg_s cfg;
    sr_read_f read;
    pthread_t th;
    struct termios oldtio;
    struct termios newtio;
    fd_set readfs;    /* file descriptor set */
    int    loop;    /* loop while TRUE */
};


/**
 * @fn static int sr_baud(uint32_t baud, struct termios *res)
 * @brief Convert the user baud to termios baud.
 * @param baud Input baud.
 * @param result The result.
 * @return -1 on error else 0
 */
static int sr_baud(uint32_t baud, struct termios *res);

/**
 * @fn static int sr_dbits(sr_dbits_et dbit, unsigned int *res)
 * @brief Convert the user stop bits to termios top bits.
 * @param dbit Input top bits.
 * @param result The result.
 * @return -1 on error else 0
 */
static int sr_dbits(sr_dbits_et dbit, unsigned int *res);

/**
 * @fn static int sr_sbits(sr_sbits_et sbit, unsigned int *res)
 * @brief Convert the user data bits to termios data bits.
 * @param sbits Input stop bits.
 * @param result The result.
 * @return -1 on error else 0
 */
static int sr_sbits(sr_sbits_et sbit, unsigned int *res);

/**
 * @fn static int sr_parity(sr_parity_et parity, unsigned int *res)
 * @brief Convert the user parity to termios parity.
 * @param parity Input parity.
 * @param result The result.
 * @return -1 on error else 0
 */
static int sr_parity(sr_parity_et parity, unsigned int *res);

/**
 * @fn static int sr_cflow(sr_cflow_et cflow, unsigned int *res)
 * @brief Convert the user flow control to termios flow control.
 * @param cflow Input flow control.
 * @param result The result.
 * @return -1 on error else 0
 */
static int sr_cflow(sr_cflow_et cflow, unsigned int *res);

/**
 * @fn static void* sr_read_cb(void* ptr)
 * @brief The reader thread callback
 * @param ptr The sr ctx.
 * @return Allways NULL.
 */
static void* sr_read_cb(void* ptr);

/**
 * @fn static void* sr_read_cb(void* ptr)
 * @brief The reader thread callback
 * @param ptr The sr ctx.
 * @return Allways NULL.
 */
static void* sr_read_cb(void* ptr);

/**
 * @fn static char* sr_prepare_buffer(struct sr_s *s, uint32_t *bytes)
 * @brief Prepare the read buffer.
 * @param sr Sr context
 * @param bytes The available bytes.
 * @raturn The allocated buffer else NULL on error.
 */
static char* sr_prepare_buffer(struct sr_s *s, uint32_t *bytes);

/**
 * @fn sr_t sr_open(struct sr_cfg_s cfg)
 * @brief Open the sr port and configure it.
 * @param cfg The sr configuration.
 * @return The sr context else NULL on error.
 */
sr_t sr_open(struct sr_cfg_s cfg) {
  struct sr_s *ctx = NULL;
  unsigned int res = 0;


  if((ctx = (struct sr_s*)malloc(sizeof(struct sr_s))) == NULL) {
    logger(LOG_ERR, "%s: Not enough memory to alloc the sr context.", __func__);
    return NULL;
  }
  memset(ctx, 0, sizeof(struct sr_s));
  ctx->magic = SR_MAGIC;
  memcpy(&ctx->cfg, &cfg, sizeof(struct sr_cfg_s));
  ctx->read = NULL;
  ctx->fd = 0;

  ctx->fd = open(ctx->cfg.dev, O_RDWR | O_NOCTTY);
  if (ctx->fd <0) {
    logger(LOG_ERR, "%s: Unable to open the dev '%s': (%d) %s", __func__, ctx->cfg.dev, errno, strerror(errno));
    sr_close(ctx);
    return NULL;
  }
  if(fcntl(ctx->fd, F_SETFL, 0) < 0)  {
    logger(LOG_ERR, "%s: Unable to call fcntl for dev '%s': (%d) %s", __func__, ctx->cfg.dev, errno, strerror(errno));
    sr_close(ctx);
    return NULL;
  }

  tcgetattr(ctx->fd,&ctx->oldtio); /* save current sr port settings */
  memset(&ctx->newtio, 0, sizeof(ctx->newtio)); /* clear struct for new port settings */

  if(sr_baud(cfg.baud, &ctx->newtio)) {
    sr_close(ctx);
    return NULL;
  }
  if(sr_dbits(cfg.dbits, &res)) {
    sr_close(ctx);
    return NULL;
  }
  if(sr_sbits(cfg.sbits, &res)) {
    sr_close(ctx);
    return NULL;
  }
  if(sr_parity(cfg.parity, &res)) {
    sr_close(ctx);
    return NULL;
  }
  if(sr_cflow(cfg.cflow, &res)) {
    sr_close(ctx);
    return NULL;
  }

  ctx->newtio.c_cflag = res;
  ctx->newtio.c_cflag |= (CLOCAL | CREAD);
    ctx->newtio.c_iflag = IGNBRK | IGNPAR | ICRNL;
  
  /* Raw output. */
  ctx->newtio.c_oflag = 0;
  ctx->newtio.c_lflag = ICANON;
  ctx->newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
  ctx->newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */

  /* now clean the modem line and activate the settings for the port */
  if(tcflush(ctx->fd, TCIFLUSH) < 0)  {
    logger(LOG_ERR, "%s: Unable to call tcfluch for dev '%s': (%d) %s", __func__, ctx->cfg.dev, errno, strerror(errno));
    sr_close(ctx);
    return NULL;
  }
  if(tcsetattr(ctx->fd, TCSANOW, &ctx->newtio) < 0)  {
    logger(LOG_ERR, "%s: Unable to call tcsetattr for dev '%s': (%d) %s", __func__, ctx->cfg.dev, errno, strerror(errno));
    sr_close(ctx);
    return NULL;
  }
  return ctx;
}

/**
 * @fn void sr_close(sr_t sr)
 * @brief Close the sr port.
 * @param sr The sr context.
 */
void sr_close(sr_t sr) {
  if(!IS_VALID(sr)) {
    logger(LOG_ERR, "%s: Invalid or null sr pointer!", __func__);
    return;
  }
  struct sr_s *s = SCAST(sr);
  if(s->fd > 0) {
    /* restore the old port settings */
    tcsetattr(s->fd, TCSANOW, &s->oldtio);
    close(s->fd);
    sr_stop_read(sr);
    /* reset magic + fd */
    memset(s, 0, sizeof(struct sr_s));
    free(s);
  }
}

/**
 * @fn int sr_start_read(sr_t sr, sr_read_f sr_read)
 * @brief Start the sr reader thread.
 * @param sr The sr context.
 * @param sr_read The reader callback
 * @return -1 on error else 0
 */
int sr_start_read(sr_t sr, sr_read_f sr_read) {
  if(!IS_VALID(sr)) {
    logger(LOG_ERR, "%s: Invalid or null sr pointer!", __func__);
    return -1;
  }
  if(!sr_read) {
    logger(LOG_ERR, "%s: Invalid or null read callback!", __func__);
    return -1;
  }
  struct sr_s *s = SCAST(sr);
  if(s->loop) {
    logger(LOG_ERR, "%s: The reader thread is already launcher!", __func__);
    return -1;
  }
  s->read = sr_read;
  s->loop = 1;
  if(pthread_create(&s->th, NULL, sr_read_cb, s)) {
    logger(LOG_ERR, "%s: Unable to start the reader thread: (%d) %s!", __func__, errno, strerror(errno));
    return -1;
  }
  if(pthread_detach(s->th)) {
    sr_close(sr);
    logger(LOG_ERR, "%s: Unable to detach the reader thread: (%d) %s!", __func__, errno, strerror(errno));
    return -1;
  }
  return 0;
}

/**
 * @fn void sr_stop_read(sr_t sr)
 * @brief Stop the sr reader thread.
 * @param sr The sr context.
 */
void sr_stop_read(sr_t sr) {
  if(!IS_VALID(sr)) {
    logger(LOG_ERR, "%s: Invalid or null sr pointer!", __func__);
    return;
  }
  struct sr_s *s = SCAST(sr);
  s->loop = 0;
  
  pthread_cancel(s->th);
  pthread_join(s->th, NULL);
  s->th = 0;
}

/**
 * @fn int sr_printf(sr_t sr, const char* fmt, ...)
 * @brief Write a command into the sr.
 * @param sr The sr context.
 * @param fmt The command format
 * @param ... The command parameters
 * @return The linux write code.
 */
int sr_printf(sr_t sr, const char* fmt, ...) {
  if(!IS_VALID(sr)) {
    logger(LOG_ERR, "%s: Invalid or null sr pointer!", __func__);
    return -1;
  }
  struct sr_s *s = SCAST(sr);
  va_list args;
  char cmd[SR_PRINTF_BUF_LEN];
  bzero(cmd, SR_PRINTF_BUF_LEN);
  va_start(args, fmt);
  vsnprintf(cmd, SR_PRINTF_BUF_LEN, fmt, args);
  va_end(args);
  return write(s->fd, cmd, strlen(cmd));
}

/**
 * @fn int sr_write(sr_t sr, const void* buffer, uint32_t length)
 * @brief Write a command into the sr.
 * @param sr The sr context.
 * @param buffer The buffer datas.
 * @param length The buffer length.
 * @return The linux write code.
 */
int sr_write(sr_t sr, const void* buffer, uint32_t length) {
  if(!IS_VALID(sr)) {
    logger(LOG_ERR, "%s: Invalid or null sr pointer!", __func__);
    return -1;
  }
  struct sr_s *s = SCAST(sr);
  return write(s->fd, buffer, length);
}

/**
 * @fn static void* sr_read_cb(void* ptr)
 * @brief The reader thread callback
 * @param ptr The sr ctx.
 * @return Allways NULL.
 */
static void* sr_read_cb(void* ptr) {
  struct sr_s *s = SCAST(ptr);
  FD_ZERO(&s->readfs);
  FD_SET(s->fd, &s->readfs);
  char *buffer;
  int32_t reads, maxfd = s->fd + 1;
  uint32_t bytes;
  
  /* loop for input */
  while (s->loop) {
    /* block until input becomes available */
    select(maxfd, &s->readfs, NULL, NULL, NULL);
    if(!FD_ISSET(s->fd, &s->readfs)) continue;

    if((buffer = sr_prepare_buffer(s, &bytes)) == NULL) {
      sleep(SR_DELAY_ON_READ_ERROR);      
      continue;
    }
    reads = read(s->fd, buffer, bytes);
    if(reads == -1) {
      logger(LOG_ERR, "%s: Unable to read datas: (%d) %s.", errno, strerror(errno), __func__);
      free(buffer);
      sleep(SR_DELAY_ON_READ_ERROR);
      continue;
    }
    buffer[reads] = 0;
    s->read(s, buffer, reads);
    free(buffer);
  }
  pthread_exit(0);
  return NULL;
}

/**
 * @fn static int sr_baud(uint32_t baud, struct termios *res)
 * @brief Convert the user baud to termios baud.
 * @param baud Input baud.
 * @param res The res.
 * @return -1 on error else 0
 */
static int sr_baud(uint32_t baud, struct termios *res) {
  switch(baud) {
    case 50:      cfsetispeed(res, B50); cfsetospeed(res, B50);           break;
    case 75:      cfsetispeed(res, B75); cfsetospeed(res, B75);           break;
    case 110:     cfsetispeed(res, B110); cfsetospeed(res, B110);         break;
    case 134:     cfsetispeed(res, B134); cfsetospeed(res, B134);         break;
    case 150:     cfsetispeed(res, B150); cfsetospeed(res, B150);         break;
    case 200:     cfsetispeed(res, B200); cfsetospeed(res, B200);         break;
    case 300:     cfsetispeed(res, B300); cfsetospeed(res, B300);         break;
    case 600:     cfsetispeed(res, B600); cfsetospeed(res, B600);         break;
    case 1200:    cfsetispeed(res, B1200); cfsetospeed(res, B1200);       break;
    case 1800:    cfsetispeed(res, B1800); cfsetospeed(res, B1800);       break;
    case 2400:    cfsetispeed(res, B2400); cfsetospeed(res, B2400);       break;
    case 4800:    cfsetispeed(res, B4800); cfsetospeed(res, B4800);       break;
    case 9600:    cfsetispeed(res, B9600); cfsetospeed(res, B9600);       break;
    case 19200:   cfsetispeed(res, B19200); cfsetospeed(res, B19200);     break;
    case 38400:   cfsetispeed(res, B38400); cfsetospeed(res, B38400);     break;
    case 57600:   cfsetispeed(res, B57600); cfsetospeed(res, B57600);     break;
    case 115200:  cfsetispeed(res, B115200); cfsetospeed(res, B115200);   break;
    case 230400:  cfsetispeed(res, B230400); cfsetospeed(res, B230400);   break;
    case 460800:  cfsetispeed(res, B460800); cfsetospeed(res, B460800);   break;
    case 500000:  cfsetispeed(res, B500000); cfsetospeed(res, B500000);   break;
    case 576000:  cfsetispeed(res, B576000); cfsetospeed(res, B576000);   break;
    case 921600:  cfsetispeed(res, B921600); cfsetospeed(res, B921600);   break;
    case 1000000: cfsetispeed(res, B1000000); cfsetospeed(res, B1000000); break;
    case 1152000: cfsetispeed(res, B1152000); cfsetospeed(res, B1152000); break;
    case 1500000: cfsetispeed(res, B1500000); cfsetospeed(res, B1500000); break;
    case 2000000: cfsetispeed(res, B2000000); cfsetospeed(res, B2000000); break;
    case 2500000: cfsetispeed(res, B2500000); cfsetospeed(res, B2500000); break;
    case 3000000: cfsetispeed(res, B3000000); cfsetospeed(res, B3000000); break;
    case 3500000: cfsetispeed(res, B3500000); cfsetospeed(res, B3500000); break;
    case 4000000: cfsetispeed(res, B4000000); cfsetospeed(res, B4000000); break;
    default:
      logger(LOG_ERR, "%s: Baud %d not supported", __func__, baud);
      return -1;
  }
  return 0;
}

/**
 * @fn static int sr_dbits(sr_dbits_et dbit, unsigned int *res)
 * @brief Convert the user stop bits to termios top bits.
 * @param dbits Input top bits.
 * @param res The res.
 * @return -1 on error else 0
 */
static int sr_dbits(sr_dbits_et dbit, unsigned int *res) {
  switch(dbit) {
    case SR_DBITS_5: 
      *res &= ~CSIZE;
      *res |= CS5;
      break;
    case SR_DBITS_6:
      *res &= ~CSIZE;
      *res |= CS6;
      break;
    case SR_DBITS_7:
      *res &= ~CSIZE;
      *res |= CS7;
      break;
    case SR_DBITS_8:
      *res &= ~CSIZE;
      *res |= CS8;
      break;
    default :
      logger(LOG_ERR, "%s: Data bits not supported %d.", __func__, dbit);
      return -1;
  }
  return 0;
}


/**
 * @fn static int sr_sbits(sr_sbits_et sbit, unsigned int *res)
 * @brief Convert the user data bits to termios data bits.
 * @param sbit Input stop bits.
 * @param res The res.
 * @return -1 on error else 0
 */
static int sr_sbits(sr_sbits_et sbit, unsigned int *res) {
  switch(sbit) {
    case SR_SBITS_1: *res &= ~CSTOPB; break;
    case SR_SBITS_2: *res |= CSTOPB; break;
    default :
      logger(LOG_ERR, "%s: Stop bits not supported %d.", __func__, sbit);
      return -1;
  }
  return 0;
}

/**
 * @fn static int sr_parity(sr_parity_et parity, unsigned int *res)
 * @brief Convert the user parity to termios parity.
 * @param parity Input parity.
 * @param res The res.
 * @return -1 on error else 0
 */
static int sr_parity(sr_parity_et parity, unsigned int *res) {
  switch(parity) {
    case SR_PARITY_NONE: 
      *res &= ~PARENB;
      break;
    case SR_PARITY_ODD:  
      *res |= PARENB;
      *res |= PARODD;
      break;
    case SR_PARITY_EVEN: 
      *res |= PARENB;
      *res &= ~PARODD;
      break;
    default :
      logger(LOG_ERR, "%s: Parity not supported %d.", __func__, parity);
      return -1;
  }
  return 0;
}

/**
 * @fn static int sr_cflow(sr_cflow_et cflow, unsigned int *res)
 * @brief Convert the user flow control to termios flow control.
 * @param cflow Input flow control.
 * @param res The res.
 * @return -1 on error else 0
 */
static int sr_cflow(sr_cflow_et cflow, unsigned int *res) {
  switch(cflow) {
    case SR_CFLOW_NONE:
      *res &= ~CRTSCTS;
      *res &= ~(IXON|IXOFF|IXANY);
      break;
    case SR_CFLOWL_XONXOFF:
      *res = (IXON|IXOFF|IXANY);
      break;
    case SR_CFLOW_RTSCTS:    
      *res = CRTSCTS;
      break;
    default :
      logger(LOG_ERR, "%s: Flow control not supported %d.", __func__, cflow);
      return -1;
  }
  return 0;
}


/**
 * @fn static char* sr_prepare_buffer(struct sr_s *s, uint32_t *bytes)
 * @brief Prepare the read buffer.
 * @param sr Sr context
 * @param bytes The available bytes.
 * @raturn The allocated buffer else NULL on error.
 */
static char* sr_prepare_buffer(struct sr_s *s, uint32_t *bytes) {
  char *buffer;

  if(ioctl(s->fd, FIONREAD, bytes) == -1) {
    logger(LOG_ERR, "%s: Unable to get the available datas: (%d) %s.", __func__, errno, strerror(errno));
    return NULL;
  }
  if(!bytes) {
    logger(LOG_ERR, "%s: Select reached without bytes (b=0)", __func__);
    return NULL;
  }
  if((buffer = (char*) malloc(*bytes)) == NULL) {
    logger(LOG_ERR, "%s: Unable to allocate memory for read buffer.", __func__);
    return NULL;
  }
  return buffer;
}
