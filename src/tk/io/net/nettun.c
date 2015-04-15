/**
*******************************************************************************
* @file netitun.c
* @author Keidan
* @date 29/10/2013
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
#include <tk/io/net/nettun.h>
#include <tk/sys/systools.h>
#include <tk/utils/string.h>
#include <tk/sys/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/if_tun.h>

/* TUNSETGROUP available in 2.6.23 */
#ifndef TUNSETGROUP
#define TUNSETGROUP   _IOW('T', 206, int)
#endif

#define TUN_DEV "/dev/net/tun"


/**
 * @fn int nettun_create(struct nettun_s* nt)
 * @brief Create a tun/tap device.
 * @param nt The device context.
 * @return -1 on error else 0 on success.
 */
int nettun_create(struct nettun_s* nt) {
  int ret = 0;
  int fd;
  struct ifreq ifr;
  struct passwd *pw;
  struct group *gr;
  uid_t owner = 0;
  gid_t group = 0;
  int type = 0;


  if(!file_exists(TUN_DEV)) {
    ret = systools_exec("mknod %s c 10 200", TUN_DEV);
    if(ret) {
      logger(LOG_ERR, "Unable to create the node %s with the following parameters: c 10 200: err=%d\n", TUN_DEV, ret); 
      return -1;
    }
  }

  if(nt->owner && strlen(nt->owner)) {
    pw = getpwnam(nt->owner);
    if(pw)
      owner = pw->pw_uid;
    else {
      owner = string_parse_int(nt->owner, -1);
      if(owner == -1){
	logger(LOG_ERR, "'%s' is neither a username nor a numeric uid.\n",
	       nt->owner);
	return -1;
      }
    }
  }
  if(nt->group && strlen(nt->group)) {
    gr = getgrnam(nt->group);
    if(gr)
      group = gr->gr_gid;
    else {
      group = string_parse_int(nt->group, -1);
      if(group == -1){
	logger(LOG_ERR, "'%s' is neither a groupame nor a numeric group.\n",
	       nt->group);
	return -1;
      }
    }
  }

  if((fd = open(TUN_DEV, O_RDWR)) < 0){
    logger(LOG_ERR, "Failed to open '%s': (%d) %s", TUN_DEV, errno, strerror(errno));
    return -1;
  }

  type = nt->type == NETTUN_TAP ? IFF_TAP : IFF_TUN;

  memset(&ifr, 0, sizeof(ifr));

  ifr.ifr_flags = type | IFF_NO_PI;
  strncpy(ifr.ifr_name, nt->name, sizeof(ifr.ifr_name) - 1);
  if(ioctl(fd, TUNSETIFF, (void *) &ifr) < 0){
    close(fd);
    logger(LOG_ERR, "TUNSETIFF failed: (%d) %s", errno, strerror(errno));
    return -1;
  }

  if(ioctl(fd, TUNSETOWNER, owner) < 0){
    close(fd);
    logger(LOG_ERR, "TUNSETOWNER failed: (%d) %s", errno, strerror(errno));
    return -1;
  }
  if(ioctl(fd, TUNSETGROUP, group) < 0){
    close(fd);
    logger(LOG_ERR, "TUNSETGROUP failed: (%d) %s", errno, strerror(errno));
    return -1;
  }

  if(ioctl(fd, TUNSETPERSIST, 1) < 0){
    close(fd);
    logger(LOG_ERR, "TUNSETPERSIST failed: (%d) %s", errno, strerror(errno));
    return -1;
  }
  close(fd);
  return 0 ;
}

/**
 * @fn int nettun_remove(struct nettun_s* nt)
 * @brief Remove the tun/tap device.
 * @param nt The device context.
 * @return -1 on error else 0 on success.
 */
int nettun_remove(struct nettun_s* nt) {
  int ret = 0;
  int fd;
  struct ifreq ifr;
  int type = 0;

  if(!file_exists(TUN_DEV)) {
    ret = systools_exec("mknod %s c 10 200", TUN_DEV);
    if(ret) {
      logger(LOG_ERR, "Unable to create the node %s with the following parameters: c 10 200: err=%d\n", TUN_DEV, ret); 
      return -1;
    }
  }

  if((fd = open(TUN_DEV, O_RDWR)) < 0){
    logger(LOG_ERR, "Failed to open '%s': (%d) %s", TUN_DEV, errno, strerror(errno));
    return -1;
  }

  type = nt->type == NETTUN_TAP ? IFF_TAP : IFF_TUN;

  memset(&ifr, 0, sizeof(ifr));

  ifr.ifr_flags = type | IFF_NO_PI;
  strncpy(ifr.ifr_name, nt->name, sizeof(ifr.ifr_name) - 1);
  if(ioctl(fd, TUNSETIFF, (void *) &ifr) < 0){
    close(fd);
    logger(LOG_ERR, "TUNSETIFF failed: (%d) %s", errno, strerror(errno));
    return -1;
  }

  if(ioctl(fd, TUNSETPERSIST, 0) < 0){
    close(fd);
    logger(LOG_ERR, "TUNSETPERSIST failed: (%d) %s", errno, strerror(errno));
    return -1;
  }
  close(fd);
  return 0;
}
