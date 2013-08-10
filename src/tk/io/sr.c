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
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <tk/text/stringtoken.h>

#define SR_MAGIC     0x532141
#define SCAST(sr)    ((struct sr_s*)sr)
#define IS_VALID(sr) (sr && SCAST(sr)->magic == SR_MAGIC)

typedef enum {
  SR_DEVICE_SET=1,
  SR_BAUD_SET=2,
  SR_DBITS_SET=4,
  SR_SBITS_SET=8,
  SR_FLOW_SET=16,
  SR_PAR_SET=24,
  SR_SET_NB=6
} sr_set_et;

struct sr_s {
    int magic;
    int fd;
    struct sr_cfg_s cfg;
    sr_read_f read;
    pthread_t th;
    struct termios oldtio;
    struct termios newtio;
    int    loop;    /* loop while TRUE */
};

struct sr_baud_s {
    speed_t ibaud;
    speed_t obaud;
};

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))
static struct sr_baud_s bauds[] = {
  {      50, B50      },
  {      75, B75      },
  {     110, B110     },
  {     134, B134     },
  {     150, B150     },
  {     200, B200     },
  {     300, B300     },
  {     600, B600     },
  {    1200, B1200    },
  {    1800, B1800    },
  {    2400, B2400    },
  {    4800, B4800    },
  {    9600, B9600    },
  {   19200, B19200   },
  {   38400, B38400   },
  {   57600, B57600   },
  {  115200, B115200  },
  {  230400, B230400  },
  {  460800, B460800  },
  {  500000, B500000  },
  {  576000, B576000  },
  {  921600, B921600  },
  { 1000000, B1000000 },
  { 1152000, B1152000 },
  { 1500000, B1500000 },
  { 2000000, B2000000 },
  { 2500000, B2500000 },
  { 3000000, B3000000 },
  { 3500000, B3500000 },
  { 4000000, B4000000 }
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
 * @fn static unsigned char* sr_prepare_buffer(struct sr_s *s, uint32_t *bytes)
 * @brief Prepare the read buffer.
 * @param sr Sr context
 * @param bytes The available bytes.
 * @raturn The allocated buffer else NULL on error.
 */
static unsigned char* sr_prepare_buffer(struct sr_s *s, uint32_t *bytes);



/**
 * @fn sr_t sr_open_from_string(const char* cfg)
 * @brief Open the sr port and configure it.
 * @param cfg The sr configuration.
 * @return The sr context else NULL on error.
 */
sr_t sr_open_from_string(const char* cfg) {
  struct sr_cfg_s cf;
  if(sr_parse_config_from_string(&cf, cfg) == -1) {
    logger(LOG_ERR, "%s: Invalid configuration!", __func__);
    return NULL;
  }
  return sr_open(cf);
}

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
  ctx->newtio.c_iflag = IGNBRK | IGNPAR;
  
  /* Raw output. */
  ctx->newtio.c_oflag = 0;
  ctx->newtio.c_cc[VTIME]    = cfg.vtime;     /* inter-character timer unused */
  ctx->newtio.c_cc[VMIN]     = cfg.vmin;     /* blocking read until 1 character arrives */
  
  /* now clean the modem line and activate the settings for the port */
  tcflush(ctx->fd, TCIFLUSH);
  if(tcsetattr(ctx->fd, TCSANOW, &ctx->newtio) < 0)  {
    logger(LOG_ERR, "%s: Unable to call tcsetattr for dev '%s': (%d) %s", __func__, ctx->cfg.dev, errno, strerror(errno));
    sr_close(ctx);
    return NULL;
  }
  return ctx;
}

/**
 * @fn void sr_get_info(sr_t sr, string_t buf)
 * @brief Get the serial informations.
 * @param sr The serial pointer.
 * @param buf The informations buffer.
 */
void sr_get_info(sr_t sr, string_t buf) {
  if(!IS_VALID(sr)) {
    logger(LOG_ERR, "%s: Invalid or null serial pointer!", __func__);
    return;
  }
  bzero(buf, sizeof(string_t));
  struct sr_s *s = SCAST(sr);
  sprintf(buf, "Serial configurtion: %s{baud=%d", s->cfg.dev, s->cfg.baud);
  
  switch(s->newtio.c_cflag & CSIZE) {
    case CS5: strcat(buf, ",dbits=5"); break;
    case CS6: strcat(buf, ",dbits=6"); break;
    case CS7: strcat(buf, ",dbits=7"); break;
    default:
    case CS8: strcat(buf, ",dbits=8"); break;
  }

  if(s->newtio.c_cflag & CSTOPB)
    strcat(buf, ",sbits=2");
  else
    strcat(buf, ",sbits=1");

  if(s->newtio.c_cflag & PARENB)
    if(s->newtio.c_cflag & PARODD) strcat(buf, ",parity=odd");
    else strcat(buf, ",parity=even");
  else strcat(buf, ",parity=none");

  if(s->newtio.c_cflag & CRTSCTS) strcat(buf, ",cflow=rts/cts");
//  else if(s->newtio.c_cflag & (IXON|IXOFF|IXANY))
//    strcat(buf, ",cflow=xon/xoff");
  else strcat(buf, ",cflow=none");
  
  if(s->newtio.c_cflag & CLOCAL) strcat(buf, ",clocal=yes");
  else strcat(buf, ",clocal=no");
  if(s->newtio.c_cflag & CREAD) strcat(buf, ",cread=yes");
  else strcat(buf, ",cread=no");
 
  if(s->newtio.c_lflag & ECHO) strcat(buf, ",echo=yes");
  else strcat(buf, ",echo=no");
  strcat(buf, "}");
}

/**
 * @fn int sr_update_vmin(sr_t sr, uint8_t vmin)
 * @brief Update the vmin value.
 * @param sr The serial pointer
 * @param vmin The new value.
 * @return -1 on error else 0
 */
int sr_update_vmin(sr_t sr, uint8_t vmin) {
  if(!IS_VALID(sr)) {
    logger(LOG_ERR, "%s: Invalid or null sr pointer!", __func__);
    return -1;
  }
  struct sr_s *s = SCAST(sr);
  struct termios t;
  memcpy(&t, &s->newtio, sizeof(struct termios));
  t.c_cc[VMIN] = vmin;
  if(tcsetattr(s->fd, TCSANOW, &t) == -1) {
    logger(LOG_ERR, "%s: Unable to change the vmin value: (%d) %s\n", __func__, errno, strerror(errno));
    return -1;
  }
  s->newtio.c_cc[VMIN] = s->cfg.vmin = vmin;
  return 0;
}

/**
 * @fn int sr_update_vtime(sr_t sr, uint8_t vtime)
 * @brief Update the vtime value.
 * @param sr The serial pointer
 * @param vtime The new value.
 * @return -1 on error else 0
 */
int sr_update_vtime(sr_t sr, uint8_t vtime) {
  if(!IS_VALID(sr)) {
    logger(LOG_ERR, "%s: Invalid or null sr pointer!", __func__);
    return -1;
  }
  struct sr_s *s = SCAST(sr);
  struct termios t;
  memcpy(&t, &s->newtio, sizeof(struct termios));
  t.c_cc[VTIME] = vtime;
  if(tcsetattr(s->fd, TCSANOW, &t) == -1) {
    logger(LOG_ERR, "%s: Unable to change the vtime value: (%d) %s\n", __func__, errno, strerror(errno));
    return -1;
  }
  s->newtio.c_cc[VTIME] = s->cfg.vtime = vtime;
  return 0;
}


/**
 * @fn int sr_update_vmin_and_vtime(sr_t sr, uint8_t vmin, uint8_t vtime)
 * @brief Update the vtime value.
 * @param sr The serial pointer
 * @param vmin The new value.
 * @param vtime The new value.
 * @return -1 on error else 0
 */
int sr_update_vmin_and_vtime(sr_t sr, uint8_t vmin, uint8_t vtime) {
  if(!IS_VALID(sr)) {
    logger(LOG_ERR, "%s: Invalid or null sr pointer!", __func__);
    return -1;
  }
  struct sr_s *s = SCAST(sr);
  struct termios t;
  memcpy(&t, &s->newtio, sizeof(struct termios));
  t.c_cc[VMIN] = vmin;
  t.c_cc[VTIME] = vtime;
  if(tcsetattr(s->fd, TCSANOW, &t) == -1) {
    logger(LOG_ERR, "%s: Unable to change the vmin and vtime value: (%d) %s\n", __func__, errno, strerror(errno));
    return -1;
  }
  s->newtio.c_cc[VMIN] = s->cfg.vmin = vmin;
  s->newtio.c_cc[VTIME] = s->cfg.vtime = vtime;
  return 0;
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
 * @fn int sr_read(sr_t sr, unsigned char* buffer, uint32_t length)
 * @brief Read a command from the sr.
 * @param sr The sr context.
 * @param buffer The buffer datas.
 * @param length The buffer length.
 * @return The linux read code.
 */
int sr_read(sr_t sr, unsigned char* buffer, uint32_t length) {
  if(!IS_VALID(sr)) {
    logger(LOG_ERR, "%s: Invalid or null sr pointer!", __func__);
    return -1;
  }
  struct sr_s *s = SCAST(sr);
  return read(s->fd, buffer, length);
}

/**
 * @fn int sr_parse_config_from_string(struct sr_cfg_s *cfg, const char* string)
 * @brief Fill the config from a string, format: dev=device:b=baud:d=data_bits:s=stop_bits:c=flowcontrol:p=parity:v=vmin:t=vtime
 * dev: serial device (eg: dev=/dev/ttyS0).
 * b: Nb bauds (eg: b=9600).
 * d: Data bits, possible values: 5, 6, 7 or 8 (eg: d=8).
 * s: Stop bits, possible values: 1 or 2 (eg: s=1).
 * c: Flow control, possible values: none, xonxoff or rtscts (eg: c:none).
 * p: Parity, possible values: none,odd or even (eg: p=none).
 * v: vmin value
 * t: vtime value
 * @param cfg The output config.
 * @param string The input config.
 * @return -1 on error else 0.
 */
int sr_parse_config_from_string(struct sr_cfg_s *cfg, const char* string) {
  int flags = 0;
  char* t, *st_name, *st_value;
  stringtoken_t tok, sub_tok;
  if(!cfg) {
    logger(LOG_ERR, "Invalid config pointer!");
    return -1;
  }
  memset(cfg, 0, sizeof(struct sr_cfg_s));
  tok = stringtoken_init(string, ":");
  if(stringtoken_count(tok) != SR_SET_NB) {
    stringtoken_release(tok);
    logger(LOG_ERR, "Invalid parameters numbers!");
    return -1;
  }
  while(stringtoken_has_more_tokens(tok)) {
    t = stringtoken_next_token(tok);
    sub_tok = stringtoken_init(t, "=");
    if(stringtoken_count(sub_tok) != 2) {
      stringtoken_release(tok);
      stringtoken_release(sub_tok);
      logger(LOG_ERR, "Invalid sub parameters numbers!");
      return -1;
    }
    st_name = stringtoken_next_token(sub_tok);
    st_value = stringtoken_next_token(sub_tok);
    if(!strcmp(st_name, "dev")) {
      strncpy(cfg->dev, st_value, SR_DEVICE_NAME_LENGTH);
      if(strlen(cfg->dev))
	flags |= SR_DEVICE_SET;
    } else if(!strcmp(st_name, "b")) {
      cfg->baud = string_parse_int(st_value, 0);
      if(cfg->baud) flags |= SR_BAUD_SET;
    } else if(!strcmp(st_name, "d")) {
      cfg->dbits = string_parse_int(st_value, 0);
      if(cfg->dbits) flags |= SR_DBITS_SET;
    } else if(!strcmp(st_name, "s")) {
      cfg->sbits = string_parse_int(st_value, 0) == 1 ? SR_SBITS_1 : SR_SBITS_2;
      if(cfg->sbits) flags |= SR_SBITS_SET;
    } else if(!strcmp(st_name, "c")) {
      if(!strcmp(st_value, "none")) {
	cfg->cflow = SR_CFLOW_NONE;
	flags |= SR_FLOW_SET;
      } else if(!strcmp(st_value, "xonxoff")) {
	cfg->cflow = SR_CFLOW_XONXOFF;
	flags |= SR_FLOW_SET;
      } else if(!strcmp(st_value, "rtscts")) {
	cfg->cflow = SR_CFLOW_RTSCTS;
	flags |= SR_FLOW_SET;
      }
    } else if(!strcmp(st_name, "p")) {
      if(!strcmp(st_value, "none")) {
	cfg->parity = SR_PARITY_NONE;
	flags |= SR_PAR_SET;
      } else if(!strcmp(st_value, "odd")) {
	cfg->parity = SR_PARITY_ODD;
	flags |= SR_PAR_SET;
      } else if(!strcmp(st_value, "even")) {
	cfg->parity = SR_PARITY_EVEN;
	flags |= SR_PAR_SET;
      }
    } else if(!strcmp(st_name, "v")) {
      cfg->vmin = string_parse_int(st_value, 254);
    } else if(!strcmp(st_name, "t")) {
      cfg->vtime = string_parse_int(st_value, 1);
    }
    stringtoken_release(sub_tok);
    free(st_name);
    free(st_value);
    free(t);
  }
  stringtoken_release(tok);
  if(!(flags & SR_DEVICE_SET)) {
    logger(LOG_ERR, "Device required!");
    return -1;
  } else if(!(flags & SR_BAUD_SET)) {
    logger(LOG_ERR, "Baud required!");
    return -1;
  } else if(!(flags & SR_DBITS_SET)) {
    logger(LOG_ERR, "Data bits required!");
    return -1;
  } else if(!(flags & SR_SBITS_SET)) {
    logger(LOG_ERR, "Stop bits required!");
    return -1;
  } else if(!(flags & SR_FLOW_SET)) {
    logger(LOG_ERR, "Flow control required!");
    return -1;
  } else if(!(flags & SR_PAR_SET)) {
    logger(LOG_ERR, "Parity required!");
    return -1;
  }
  return 0;
}

/**
 * @fn static void* sr_read_cb(void* ptr)
 * @brief The reader thread callback
 * @param ptr The sr ctx.
 * @return Allways NULL.
 */
static void* sr_read_cb(void* ptr) {
  struct sr_s *s = SCAST(ptr);
  unsigned char *buffer;
  uint32_t bytes;
  logger(LOG_INFO, "%s: Wait on port %d for device message.", __func__, s->fd);
  /* loop for input */
  while (s->loop) {
    
    /* block until input becomes available */
    if((buffer = sr_prepare_buffer(s, &bytes)) == NULL) {
      sleep(SR_DELAY_ON_READ_ERROR);      
      continue;
    }
    int reads = read(s->fd, buffer, bytes);
    if(reads == -1) {
      logger(LOG_ERR, "%s: Unable to read datas: (%d) %s.", __func__, errno, strerror(errno));
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
  int i = 0;
  for(;i < ARRAY_LENGTH(bauds); i++)
    if(bauds[i].ibaud == baud) {
      cfsetispeed(res, bauds[i].obaud);
      cfsetospeed(res, bauds[i].obaud);
      return 0;
    }
  logger(LOG_ERR, "%s: Baud %d not supported", __func__, baud);
  return -1;
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
      //*res &= ~CSIZE;
      *res |= CS5;
      break;
    case SR_DBITS_6:
      //*res &= ~CSIZE;
      *res |= CS6;
      break;
    case SR_DBITS_7:
      //*res &= ~CSIZE;
      *res |= CS7;
      break;
    case SR_DBITS_8:
      //*res &= ~CSIZE;
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
    case SR_SBITS_1: /**res &= ~CSTOPB;*/ break;
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
      //*res &= ~PARENB;
      break;
    case SR_PARITY_ODD:  
      *res |= PARENB;
      *res |= PARODD;
      break;
    case SR_PARITY_EVEN: 
      *res |= PARENB;
      //*res &= ~PARODD;
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
      //if((*res & CRTSCTS)) *res &= ~CRTSCTS;
      //if((*res & (IXON|IXOFF|IXANY))) *res &= ~(IXON|IXOFF|IXANY);
      break;
    case SR_CFLOW_XONXOFF:
      if(!(*res & (IXON|IXOFF|IXANY))) *res |= (IXON|IXOFF|IXANY);
      break;
    case SR_CFLOW_RTSCTS:    
      if(!(*res & CRTSCTS)) *res |= CRTSCTS;
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
static unsigned char* sr_prepare_buffer(struct sr_s *s, uint32_t *bytes) {
  unsigned char *buffer;
  *bytes = s->newtio.c_cc[VMIN];
  if((buffer = (unsigned char*) malloc(*bytes)) == NULL) {
    logger(LOG_ERR, "%s: Unable to allocate memory for read buffer.", __func__);
    return NULL;
  }
  return buffer;
}

