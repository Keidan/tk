#ifndef __NETIFACE_STATUS_H__
  #define __NETIFACE_STATUS_H__

  #define PROCFS_NET_WIRELESS "/proc/net/wireless"
  #define PROCFS_NET_DEV "/proc/net/dev"

  struct discarded_packets_s {
      int nwid;
      int crypt;
      int frag;
      int retry;
      int misc;
  };
					
  struct quality_s {
      int link;
      int level;
      int noise;
  };
					
  struct internal_s {
      _Bool wireless;
      int status;
      int missed_beacon;
      int we21;
  };

  struct wireless_s {
      struct discarded_packets_s discarded_packets;
      struct quality_s quality;
      struct internal_s internal;
  };
				
  struct packet_rx_s {
      unsigned long long packets;
      unsigned long long bytes;
      unsigned long errors;
      unsigned long dropped;
      unsigned long compressed;
      unsigned long fifo_errors;
      unsigned long frame_errors; /* recv'd frame alignment error */
      unsigned long multicast;	/* multicast packets received   */
  };
					
  struct packet_tx_s {
      unsigned long long packets;
      unsigned long long bytes;
      unsigned long errors;
      unsigned long dropped;
      unsigned long compressed;
      unsigned long fifo_errors;
      unsigned long collisions;
      unsigned long carrier_errors;
  };

  struct packets_s {
      _Bool valid;
      struct packet_rx_s rx;
      struct packet_tx_s tx;
  };

  struct iface_status_s {
    struct wireless_s wireless;
    struct packets_s packets;
  };

  /**
   * @fn int netiface_status_load_wireless(const char* iface_name, struct wireless_s *wireless)
   * @brief Load the wireless informations.
   * @param iface_name The iface name.
   * @param wireless The result informations.
   * @return -1 on error else 0 on success.
   */
  int netiface_status_load_wireless(const char* iface_name, struct wireless_s *wireless);

  /**
   * @fn int netiface_status_load_packets(const char* iface_name, struct packets_s *packets)
   * @brief Load the packets informations.
   * @param iface_name The iface name.
   * @param packets The result informations.
   * @return -1 on error else 0 on success.
   */
  int netiface_status_load_packets(const char* iface_name, struct packets_s *packets);
#endif /* __NETIFACE_STATUS_H__ */
