/**
 *******************************************************************************
 * @file sr.h
 * @author Keidan
 * @date 03/01/2013
 * @par Project
 * tk
 *
 * @par Copyright
 * Copyright 2011-2013 Keidan, all right reserved
 *
 * This software is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY.
 *
 * Licence summary : 
 *    You can modify and redistribute the sources code and binaries.
 *    You can send me the bug-fix
 *
 * Term of the licence in in the file licence.txt.
 *
 *******************************************************************************
 */
#ifndef __SR_H__
  #define __SR_H__

  #include <stdint.h>

  #define SR_DEVICE_NAME_LENGTH      0x10
  #define SR_PRINTF_BUF_LEN          0xFF
  #define SR_DELAY_ON_READ_ERROR     0x05


  #define CFLOW_STR(cf) (cf == SR_CFLOW_NONE ? "none" : (cf == SR_CFLOW_XONXOFF ? "xonxoff" : (cf == SR_CFLOW_RTSCTS ? "rtscts" : "unknown")))

  #define PARITY_STR(p) (p == SR_PARITY_NONE ? "none" : (p == SR_PARITY_ODD ? "odd" : (p == SR_PARITY_EVEN ? "even" : "unknown")))

  typedef enum { 
    SR_DBITS_5=5,
    SR_DBITS_6,
    SR_DBITS_7,
    SR_DBITS_8
  } sr_dbits_et;

  typedef enum { 
    SR_SBITS_1,
    SR_SBITS_2,
  } sr_sbits_et;

  typedef enum {
    SR_CFLOW_NONE = 0,
    SR_CFLOW_XONXOFF,
    SR_CFLOW_RTSCTS
  } sr_cflow_et;

  typedef enum {
    SR_PARITY_NONE=0,
    SR_PARITY_ODD,
    SR_PARITY_EVEN
  } sr_parity_et;

  typedef void* sr_t;

  typedef void (*sr_read_f)(sr_t sr, unsigned char* buffer, uint32_t length);

  struct sr_cfg_s {
      char dev[SR_DEVICE_NAME_LENGTH];
      uint32_t baud;
      sr_dbits_et dbits;
      sr_sbits_et sbits;
      sr_cflow_et cflow;
      sr_parity_et parity;
  };

  /**
   * @fn sr_t sr_open(struct sr_cfg_s cfg)
   * @brief Open the sr port and configure it.
   * @param cfg The sr configuration.
   * @return The sr context else NULL on error.
   */
  sr_t sr_open(struct sr_cfg_s cfg);

  /**
   * @fn sr_t sr_open_from_string(const char* cfg)
   * @brief Open the sr port and configure it.
   * @param cfg The sr configuration.
   * @return The sr context else NULL on error.
   */
  sr_t sr_open_from_string(const char* cfg);

  /**
   * @fn void sr_close(sr_t sr)
   * @brief Close the sr port.
   * @param sr The sr context.
   */
  void sr_close(sr_t sr);

  /**
   * @fn int sr_parse_config_from_string(struct sr_cfg_s *cfg, const char* string)
   * @brief Fill the config from a string, format: dev=device:b=baud:d=data_bits:s=stop_bits:c=flowcontrol:p=parity
   * dev: serial device (eg: dev=/dev/ttyS0).
   * b: Nb bauds (eg: b=9600).
   * d: Data bits, possible values: 5, 6, 7 or 8 (eg: d=8).
   * s: Stop bits, possible values: 1 or 2 (eg: s=1).
   * c: Flow control, possible values: none, xonxoff or rtscts (eg: c:none).
   * p: Parity, possible values: none,odd or even (eg: p=none).
   * @param cfg The output config.
   * @param string The input config.
   * @return -1 on error else 0.
   */
  int sr_parse_config_from_string(struct sr_cfg_s *cfg, const char* string);

  /**
   * @fn int sr_start_read(sr_t sr, sr_read_f sr_read)
   * @brief Start the sr reader thread.
   * @param sr The sr context.
   * @param sr_read The reader callback
   * @return -1 on error else 0.
   */
  int sr_start_read(sr_t sr, sr_read_f sr_read);

  /**
   * @fn sr_stop_read(sr_t sr)
   * @brief Stop the sr reader thread.
   * @param sr The sr context.
   */
  void sr_stop_read(sr_t sr);

  /**
   * @fn int sr_printf(sr_t sr, const char* fmt, ...)
   * @brief Write a command into the sr.
   * @param sr The sr context.
   * @param fmt The command format
   * @param ... The command parameters
   * @return -1 on error else 0
   */
  int sr_printf(sr_t sr, const char* fmt, ...);

  /**
   * @fn int sr_write(sr_t sr, const void* buffer, uint32_t length)
   * @brief Write a command into the sr.
   * @param sr The sr context.
   * @param buffer The buffer datas.
   * @param length The buffer length.
   * @return -1 on error else 0.
   */
  int sr_write(sr_t sr, const void* buffer, uint32_t length);

  /**
   * @fn int sr_read(sr_t sr, unsigned char* buffer, uint32_t length)
   * @brief Read a command from the sr.
   * @param sr The sr context.
   * @param buffer The buffer datas.
   * @param length The buffer length.
   * @return The linux read code.
   */
  int sr_read(sr_t sr, unsigned char* buffer, uint32_t length);

#endif /* __SR_H__ */
