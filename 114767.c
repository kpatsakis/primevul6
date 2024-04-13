void ndpi_set_protocol_detection_bitmask2(struct ndpi_detection_module_struct *ndpi_str,
                                          const NDPI_PROTOCOL_BITMASK *dbm) {
  NDPI_PROTOCOL_BITMASK detection_bitmask_local;
  NDPI_PROTOCOL_BITMASK *detection_bitmask = &detection_bitmask_local;
  u_int32_t a = 0;

  NDPI_BITMASK_SET(detection_bitmask_local, *dbm);
  NDPI_BITMASK_SET(ndpi_str->detection_bitmask, *dbm);

  /* set this here to zero to be interrupt safe */
  ndpi_str->callback_buffer_size = 0;

  /* HTTP */
  init_http_dissector(ndpi_str, &a, detection_bitmask);

  /* STARCRAFT */
  init_starcraft_dissector(ndpi_str, &a, detection_bitmask);

  /* TLS */
  init_tls_dissector(ndpi_str, &a, detection_bitmask);

  /* STUN */
  init_stun_dissector(ndpi_str, &a, detection_bitmask);

  /* RTP */
  init_rtp_dissector(ndpi_str, &a, detection_bitmask);

  /* RTSP */
  init_rtsp_dissector(ndpi_str, &a, detection_bitmask);

  /* RDP */
  init_rdp_dissector(ndpi_str, &a, detection_bitmask);

  /* SIP */
  init_sip_dissector(ndpi_str, &a, detection_bitmask);

  /* IMO */
  init_imo_dissector(ndpi_str, &a, detection_bitmask);

  /* Teredo */
  init_teredo_dissector(ndpi_str, &a, detection_bitmask);

  /* EDONKEY */
  init_edonkey_dissector(ndpi_str, &a, detection_bitmask);

  /* FASTTRACK */
  init_fasttrack_dissector(ndpi_str, &a, detection_bitmask);

  /* GNUTELLA */
  init_gnutella_dissector(ndpi_str, &a, detection_bitmask);

  /* DIRECTCONNECT */
  init_directconnect_dissector(ndpi_str, &a, detection_bitmask);

  /* NATS */
  init_nats_dissector(ndpi_str, &a, detection_bitmask);

  /* YAHOO */
  init_yahoo_dissector(ndpi_str, &a, detection_bitmask);

  /* OSCAR */
  init_oscar_dissector(ndpi_str, &a, detection_bitmask);

  /* APPLEJUICE */
  init_applejuice_dissector(ndpi_str, &a, detection_bitmask);

  /* SOULSEEK */
  init_soulseek_dissector(ndpi_str, &a, detection_bitmask);

  /* SOCKS */
  init_socks_dissector(ndpi_str, &a, detection_bitmask);

  /* IRC */
  init_irc_dissector(ndpi_str, &a, detection_bitmask);

  /* JABBER */
  init_jabber_dissector(ndpi_str, &a, detection_bitmask);

  /* MAIL_POP */
  init_mail_pop_dissector(ndpi_str, &a, detection_bitmask);

  /* MAIL_IMAP */
  init_mail_imap_dissector(ndpi_str, &a, detection_bitmask);

  /* MAIL_SMTP */
  init_mail_smtp_dissector(ndpi_str, &a, detection_bitmask);

  /* USENET */
  init_usenet_dissector(ndpi_str, &a, detection_bitmask);

  /* DNS */
  init_dns_dissector(ndpi_str, &a, detection_bitmask);

  /* FILETOPIA */
  init_fbzero_dissector(ndpi_str, &a, detection_bitmask);

  /* VMWARE */
  init_vmware_dissector(ndpi_str, &a, detection_bitmask);

  /* NON_TCP_UDP */
  init_non_tcp_udp_dissector(ndpi_str, &a, detection_bitmask);

  /* TVANTS */
  init_tvants_dissector(ndpi_str, &a, detection_bitmask);

  /* SOPCAST */
  init_sopcast_dissector(ndpi_str, &a, detection_bitmask);

  /* TVUPLAYER */
  init_tvuplayer_dissector(ndpi_str, &a, detection_bitmask);

  /* PPSTREAM */
  init_ppstream_dissector(ndpi_str, &a, detection_bitmask);

  /* PPLIVE */
  init_pplive_dissector(ndpi_str, &a, detection_bitmask);

  /* IAX */
  init_iax_dissector(ndpi_str, &a, detection_bitmask);

  /* MGPC */
  init_mgpc_dissector(ndpi_str, &a, detection_bitmask);

  /* ZATTOO */
  init_zattoo_dissector(ndpi_str, &a, detection_bitmask);

  /* QQ */
  init_qq_dissector(ndpi_str, &a, detection_bitmask);

  /* SSH */
  init_ssh_dissector(ndpi_str, &a, detection_bitmask);

  /* AYIYA */
  init_ayiya_dissector(ndpi_str, &a, detection_bitmask);

  /* THUNDER */
  init_thunder_dissector(ndpi_str, &a, detection_bitmask);

  /* VNC */
  init_vnc_dissector(ndpi_str, &a, detection_bitmask);

  /* TEAMVIEWER */
  init_teamviewer_dissector(ndpi_str, &a, detection_bitmask);

  /* DHCP */
  init_dhcp_dissector(ndpi_str, &a, detection_bitmask);

  /* STEAM */
  init_steam_dissector(ndpi_str, &a, detection_bitmask);

  /* HALFLIFE2 */
  init_halflife2_dissector(ndpi_str, &a, detection_bitmask);

  /* XBOX */
  init_xbox_dissector(ndpi_str, &a, detection_bitmask);

  /* HTTP_APPLICATION_ACTIVESYNC */
  init_http_activesync_dissector(ndpi_str, &a, detection_bitmask);

  /* SMB */
  init_smb_dissector(ndpi_str, &a, detection_bitmask);

  /* MINING */
  init_mining_dissector(ndpi_str, &a, detection_bitmask);

  /* TELNET */
  init_telnet_dissector(ndpi_str, &a, detection_bitmask);

  /* NTP */
  init_ntp_dissector(ndpi_str, &a, detection_bitmask);

  /* NFS */
  init_nfs_dissector(ndpi_str, &a, detection_bitmask);

  /* SSDP */
  init_ssdp_dissector(ndpi_str, &a, detection_bitmask);

  /* WORLD_OF_WARCRAFT */
  init_world_of_warcraft_dissector(ndpi_str, &a, detection_bitmask);

  /* POSTGRES */
  init_postgres_dissector(ndpi_str, &a, detection_bitmask);

  /* MYSQL */
  init_mysql_dissector(ndpi_str, &a, detection_bitmask);

  /* BGP */
  init_bgp_dissector(ndpi_str, &a, detection_bitmask);

  /* BATTLEFIELD */
  init_battlefield_dissector(ndpi_str, &a, detection_bitmask);

  /* PCANYWHERE */
  init_pcanywhere_dissector(ndpi_str, &a, detection_bitmask);

  /* SNMP */
  init_snmp_dissector(ndpi_str, &a, detection_bitmask);

  /* KONTIKI */
  init_kontiki_dissector(ndpi_str, &a, detection_bitmask);

  /* ICECAST */
  init_icecast_dissector(ndpi_str, &a, detection_bitmask);

  /* SHOUTCAST */
  init_shoutcast_dissector(ndpi_str, &a, detection_bitmask);

  /* KERBEROS */
  init_kerberos_dissector(ndpi_str, &a, detection_bitmask);

  /* OPENFT */
  init_openft_dissector(ndpi_str, &a, detection_bitmask);

  /* SYSLOG */
  init_syslog_dissector(ndpi_str, &a, detection_bitmask);

  /* DIRECT_DOWNLOAD_LINK */
  init_directdownloadlink_dissector(ndpi_str, &a, detection_bitmask);

  /* NETBIOS */
  init_netbios_dissector(ndpi_str, &a, detection_bitmask);

  /* MDNS */
  init_mdns_dissector(ndpi_str, &a, detection_bitmask);

  /* IPP */
  init_ipp_dissector(ndpi_str, &a, detection_bitmask);

  /* LDAP */
  init_ldap_dissector(ndpi_str, &a, detection_bitmask);

  /* WARCRAFT3 */
  init_warcraft3_dissector(ndpi_str, &a, detection_bitmask);

  /* XDMCP */
  init_xdmcp_dissector(ndpi_str, &a, detection_bitmask);

  /* TFTP */
  init_tftp_dissector(ndpi_str, &a, detection_bitmask);

  /* MSSQL_TDS */
  init_mssql_tds_dissector(ndpi_str, &a, detection_bitmask);

  /* PPTP */
  init_pptp_dissector(ndpi_str, &a, detection_bitmask);

  /* STEALTHNET */
  init_stealthnet_dissector(ndpi_str, &a, detection_bitmask);

  /* DHCPV6 */
  init_dhcpv6_dissector(ndpi_str, &a, detection_bitmask);

  /* AFP */
  init_afp_dissector(ndpi_str, &a, detection_bitmask);

  /* check_mk */
  init_checkmk_dissector(ndpi_str, &a, detection_bitmask);

  /* AIMINI */
  init_aimini_dissector(ndpi_str, &a, detection_bitmask);

  /* FLORENSIA */
  init_florensia_dissector(ndpi_str, &a, detection_bitmask);

  /* MAPLESTORY */
  init_maplestory_dissector(ndpi_str, &a, detection_bitmask);

  /* DOFUS */
  init_dofus_dissector(ndpi_str, &a, detection_bitmask);

  /* WORLD_OF_KUNG_FU */
  init_world_of_kung_fu_dissector(ndpi_str, &a, detection_bitmask);

  /* FIESTA */
  init_fiesta_dissector(ndpi_str, &a, detection_bitmask);

  /* CROSSIFIRE */
  init_crossfire_dissector(ndpi_str, &a, detection_bitmask);

  /* GUILDWARS */
  init_guildwars_dissector(ndpi_str, &a, detection_bitmask);

  /* ARMAGETRON */
  init_armagetron_dissector(ndpi_str, &a, detection_bitmask);

  /* DROPBOX */
  init_dropbox_dissector(ndpi_str, &a, detection_bitmask);

  /* SPOTIFY */
  init_spotify_dissector(ndpi_str, &a, detection_bitmask);

  /* RADIUS */
  init_radius_dissector(ndpi_str, &a, detection_bitmask);

  /* CITRIX */
  init_citrix_dissector(ndpi_str, &a, detection_bitmask);

  /* LOTUS_NOTES */
  init_lotus_notes_dissector(ndpi_str, &a, detection_bitmask);

  /* GTP */
  init_gtp_dissector(ndpi_str, &a, detection_bitmask);

  /* DCERPC */
  init_dcerpc_dissector(ndpi_str, &a, detection_bitmask);

  /* NETFLOW */
  init_netflow_dissector(ndpi_str, &a, detection_bitmask);

  /* SFLOW */
  init_sflow_dissector(ndpi_str, &a, detection_bitmask);

  /* H323 */
  init_h323_dissector(ndpi_str, &a, detection_bitmask);

  /* OPENVPN */
  init_openvpn_dissector(ndpi_str, &a, detection_bitmask);

  /* NOE */
  init_noe_dissector(ndpi_str, &a, detection_bitmask);

  /* CISCOVPN */
  init_ciscovpn_dissector(ndpi_str, &a, detection_bitmask);

  /* TEAMSPEAK */
  init_teamspeak_dissector(ndpi_str, &a, detection_bitmask);

  /* TOR */
  init_tor_dissector(ndpi_str, &a, detection_bitmask);

  /* SKINNY */
  init_skinny_dissector(ndpi_str, &a, detection_bitmask);

  /* RTCP */
  init_rtcp_dissector(ndpi_str, &a, detection_bitmask);

  /* RSYNC */
  init_rsync_dissector(ndpi_str, &a, detection_bitmask);

  /* WHOIS_DAS */
  init_whois_das_dissector(ndpi_str, &a, detection_bitmask);

  /* ORACLE */
  init_oracle_dissector(ndpi_str, &a, detection_bitmask);

  /* CORBA */
  init_corba_dissector(ndpi_str, &a, detection_bitmask);

  /* RTMP */
  init_rtmp_dissector(ndpi_str, &a, detection_bitmask);

  /* FTP_CONTROL */
  init_ftp_control_dissector(ndpi_str, &a, detection_bitmask);

  /* FTP_DATA */
  init_ftp_data_dissector(ndpi_str, &a, detection_bitmask);

  /* PANDO */
  init_pando_dissector(ndpi_str, &a, detection_bitmask);

  /* MEGACO */
  init_megaco_dissector(ndpi_str, &a, detection_bitmask);

  /* REDIS */
  init_redis_dissector(ndpi_str, &a, detection_bitmask);

  /* UPnP */
  init_upnp_dissector(ndpi_str, &a, detection_bitmask);

  /* VHUA */
  init_vhua_dissector(ndpi_str, &a, detection_bitmask);

  /* ZMQ */
  init_zmq_dissector(ndpi_str, &a, detection_bitmask);

  /* TELEGRAM */
  init_telegram_dissector(ndpi_str, &a, detection_bitmask);

  /* QUIC */
  init_quic_dissector(ndpi_str, &a, detection_bitmask);

  /* DIAMETER */
  init_diameter_dissector(ndpi_str, &a, detection_bitmask);

  /* APPLE_PUSH */
  init_apple_push_dissector(ndpi_str, &a, detection_bitmask);

  /* EAQ */
  init_eaq_dissector(ndpi_str, &a, detection_bitmask);

  /* KAKAOTALK_VOICE */
  init_kakaotalk_voice_dissector(ndpi_str, &a, detection_bitmask);

  /* MPEGTS */
  init_mpegts_dissector(ndpi_str, &a, detection_bitmask);

  /* UBNTAC2 */
  init_ubntac2_dissector(ndpi_str, &a, detection_bitmask);

  /* COAP */
  init_coap_dissector(ndpi_str, &a, detection_bitmask);

  /* MQTT */
  init_mqtt_dissector(ndpi_str, &a, detection_bitmask);

  /* SOME/IP */
  init_someip_dissector(ndpi_str, &a, detection_bitmask);

  /* RX */
  init_rx_dissector(ndpi_str, &a, detection_bitmask);

  /* GIT */
  init_git_dissector(ndpi_str, &a, detection_bitmask);

  /* HANGOUT */
  init_hangout_dissector(ndpi_str, &a, detection_bitmask);

  /* DRDA */
  init_drda_dissector(ndpi_str, &a, detection_bitmask);

  /* BJNP */
  init_bjnp_dissector(ndpi_str, &a, detection_bitmask);

  /* SMPP */
  init_smpp_dissector(ndpi_str, &a, detection_bitmask);

  /* TINC */
  init_tinc_dissector(ndpi_str, &a, detection_bitmask);

  /* FIX */
  init_fix_dissector(ndpi_str, &a, detection_bitmask);

  /* NINTENDO */
  init_nintendo_dissector(ndpi_str, &a, detection_bitmask);

  /* MODBUS */
  init_modbus_dissector(ndpi_str, &a, detection_bitmask);

  /* CAPWAP */
  init_capwap_dissector(ndpi_str, &a, detection_bitmask);

  /* ZABBIX */
  init_zabbix_dissector(ndpi_str, &a, detection_bitmask);

  /*** Put false-positive sensitive protocols at the end ***/

  /* VIBER */
  init_viber_dissector(ndpi_str, &a, detection_bitmask);

  /* SKYPE */
  init_skype_dissector(ndpi_str, &a, detection_bitmask);

  /* BITTORRENT */
  init_bittorrent_dissector(ndpi_str, &a, detection_bitmask);

  /* WHATSAPP */
  init_whatsapp_dissector(ndpi_str, &a, detection_bitmask);

  /* OOKLA */
  init_ookla_dissector(ndpi_str, &a, detection_bitmask);

  /* AMQP */
  init_amqp_dissector(ndpi_str, &a, detection_bitmask);

  /* CSGO */
  init_csgo_dissector(ndpi_str, &a, detection_bitmask);

  /* LISP */
  init_lisp_dissector(ndpi_str, &a, detection_bitmask);

  /* AJP */
  init_ajp_dissector(ndpi_str, &a, detection_bitmask);

  /* Memcached */
  init_memcached_dissector(ndpi_str, &a, detection_bitmask);

  /* Nest Log Sink */
  init_nest_log_sink_dissector(ndpi_str, &a, detection_bitmask);

  /* WireGuard VPN */
  init_wireguard_dissector(ndpi_str, &a, detection_bitmask);

  /* Amazon_Video */
  init_amazon_video_dissector(ndpi_str, &a, detection_bitmask);

  /* Targus Getdata */
  init_targus_getdata_dissector(ndpi_str, &a, detection_bitmask);

  /* S7 comm */
  init_s7comm_dissector(ndpi_str, &a, detection_bitmask);

  /* IEC 60870-5-104 */
  init_104_dissector(ndpi_str, &a, detection_bitmask);

  /* WEBSOCKET */
  init_websocket_dissector(ndpi_str, &a, detection_bitmask);

#ifdef CUSTOM_NDPI_PROTOCOLS
#include "../../../nDPI-custom/custom_ndpi_main_init.c"
#endif

  /* ----------------------------------------------------------------- */

  ndpi_str->callback_buffer_size = a;

  NDPI_LOG_DBG2(ndpi_str, "callback_buffer_size is %u\n", ndpi_str->callback_buffer_size);

  /* now build the specific buffer for tcp, udp and non_tcp_udp */
  ndpi_str->callback_buffer_size_tcp_payload = 0;
  ndpi_str->callback_buffer_size_tcp_no_payload = 0;
  for (a = 0; a < ndpi_str->callback_buffer_size; a++) {
    if((ndpi_str->callback_buffer[a].ndpi_selection_bitmask &
	(NDPI_SELECTION_BITMASK_PROTOCOL_INT_TCP | NDPI_SELECTION_BITMASK_PROTOCOL_INT_TCP_OR_UDP |
	 NDPI_SELECTION_BITMASK_PROTOCOL_COMPLETE_TRAFFIC)) != 0) {
      if(_ndpi_debug_callbacks)
	NDPI_LOG_DBG2(ndpi_str, "callback_buffer_tcp_payload, adding buffer %u as entry %u\n", a,
		      ndpi_str->callback_buffer_size_tcp_payload);

      memcpy(&ndpi_str->callback_buffer_tcp_payload[ndpi_str->callback_buffer_size_tcp_payload],
	     &ndpi_str->callback_buffer[a], sizeof(struct ndpi_call_function_struct));
      ndpi_str->callback_buffer_size_tcp_payload++;

      if((ndpi_str->callback_buffer[a].ndpi_selection_bitmask & NDPI_SELECTION_BITMASK_PROTOCOL_HAS_PAYLOAD) ==
	 0) {
	if(_ndpi_debug_callbacks)
	  NDPI_LOG_DBG2(
                        ndpi_str,
                        "\tcallback_buffer_tcp_no_payload, additional adding buffer %u to no_payload process\n", a);

	memcpy(&ndpi_str->callback_buffer_tcp_no_payload[ndpi_str->callback_buffer_size_tcp_no_payload],
	       &ndpi_str->callback_buffer[a], sizeof(struct ndpi_call_function_struct));
	ndpi_str->callback_buffer_size_tcp_no_payload++;
      }
    }
  }

  ndpi_str->callback_buffer_size_udp = 0;
  for (a = 0; a < ndpi_str->callback_buffer_size; a++) {
    if((ndpi_str->callback_buffer[a].ndpi_selection_bitmask &
	(NDPI_SELECTION_BITMASK_PROTOCOL_INT_UDP | NDPI_SELECTION_BITMASK_PROTOCOL_INT_TCP_OR_UDP |
	 NDPI_SELECTION_BITMASK_PROTOCOL_COMPLETE_TRAFFIC)) != 0) {
      if(_ndpi_debug_callbacks)
	NDPI_LOG_DBG2(ndpi_str, "callback_buffer_size_udp: adding buffer : %u as entry %u\n", a,
		      ndpi_str->callback_buffer_size_udp);

      memcpy(&ndpi_str->callback_buffer_udp[ndpi_str->callback_buffer_size_udp], &ndpi_str->callback_buffer[a],
	     sizeof(struct ndpi_call_function_struct));
      ndpi_str->callback_buffer_size_udp++;
    }
  }

  ndpi_str->callback_buffer_size_non_tcp_udp = 0;
  for (a = 0; a < ndpi_str->callback_buffer_size; a++) {
    if((ndpi_str->callback_buffer[a].ndpi_selection_bitmask &
	(NDPI_SELECTION_BITMASK_PROTOCOL_INT_TCP | NDPI_SELECTION_BITMASK_PROTOCOL_INT_UDP |
	 NDPI_SELECTION_BITMASK_PROTOCOL_INT_TCP_OR_UDP)) == 0 ||
       (ndpi_str->callback_buffer[a].ndpi_selection_bitmask & NDPI_SELECTION_BITMASK_PROTOCOL_COMPLETE_TRAFFIC) !=
       0) {
      if(_ndpi_debug_callbacks)
	NDPI_LOG_DBG2(ndpi_str, "callback_buffer_non_tcp_udp: adding buffer : %u as entry %u\n", a,
		      ndpi_str->callback_buffer_size_non_tcp_udp);

      memcpy(&ndpi_str->callback_buffer_non_tcp_udp[ndpi_str->callback_buffer_size_non_tcp_udp],
	     &ndpi_str->callback_buffer[a], sizeof(struct ndpi_call_function_struct));
      ndpi_str->callback_buffer_size_non_tcp_udp++;
    }
  }
}