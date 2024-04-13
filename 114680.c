static void ndpi_init_protocol_defaults(struct ndpi_detection_module_struct *ndpi_str) {
  ndpi_port_range ports_a[MAX_DEFAULT_PORTS], ports_b[MAX_DEFAULT_PORTS];
  u_int16_t no_master[2] = {NDPI_PROTOCOL_NO_MASTER_PROTO, NDPI_PROTOCOL_NO_MASTER_PROTO}, custom_master[2];

  /* Reset all settings */
  memset(ndpi_str->proto_defaults, 0, sizeof(ndpi_str->proto_defaults));

  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_UNRATED, NDPI_PROTOCOL_UNKNOWN, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Unknown", NDPI_PROTOCOL_CATEGORY_UNSPECIFIED,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_UNSAFE, NDPI_PROTOCOL_FTP_CONTROL, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "FTP_CONTROL", NDPI_PROTOCOL_CATEGORY_DOWNLOAD_FT,
			  ndpi_build_default_ports(ports_a, 21, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_FTP_DATA, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "FTP_DATA", NDPI_PROTOCOL_CATEGORY_DOWNLOAD_FT,
			  ndpi_build_default_ports(ports_a, 20, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_UNSAFE, NDPI_PROTOCOL_MAIL_POP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "POP3", NDPI_PROTOCOL_CATEGORY_MAIL,
			  ndpi_build_default_ports(ports_a, 110, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_SAFE, NDPI_PROTOCOL_MAIL_POPS, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "POPS", NDPI_PROTOCOL_CATEGORY_MAIL,
			  ndpi_build_default_ports(ports_a, 995, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_MAIL_SMTP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "SMTP", NDPI_PROTOCOL_CATEGORY_MAIL,
			  ndpi_build_default_ports(ports_a, 25, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_SAFE, NDPI_PROTOCOL_MAIL_SMTPS, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "SMTPS", NDPI_PROTOCOL_CATEGORY_MAIL,
			  ndpi_build_default_ports(ports_a, 465, 587, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_UNSAFE, NDPI_PROTOCOL_MAIL_IMAP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "IMAP", NDPI_PROTOCOL_CATEGORY_MAIL,
			  ndpi_build_default_ports(ports_a, 143, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_SAFE, NDPI_PROTOCOL_MAIL_IMAPS, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "IMAPS", NDPI_PROTOCOL_CATEGORY_MAIL,
			  ndpi_build_default_ports(ports_a, 993, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_DNS, 1 /* can_have_a_subprotocol */,
			  no_master, no_master, "DNS", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 53, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 53, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_IPP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "IPP", NDPI_PROTOCOL_CATEGORY_SYSTEM_OS,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_IMO, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "IMO", NDPI_PROTOCOL_CATEGORY_VOIP,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_HTTP, 1 /* can_have_a_subprotocol */,
			  no_master, no_master, "HTTP", NDPI_PROTOCOL_CATEGORY_WEB,
			  ndpi_build_default_ports(ports_a, 80, 0 /* ntop */, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_MDNS, 1 /* can_have_a_subprotocol */,
			  no_master, no_master, "MDNS", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 5353, 5354, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_NTP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "NTP", NDPI_PROTOCOL_CATEGORY_SYSTEM_OS,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 123, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_NETBIOS, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "NetBIOS", NDPI_PROTOCOL_CATEGORY_SYSTEM_OS,
			  ndpi_build_default_ports(ports_a, 139, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 137, 138, 139, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_NFS, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "NFS", NDPI_PROTOCOL_CATEGORY_DATA_TRANSFER,
			  ndpi_build_default_ports(ports_a, 2049, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 2049, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_SSDP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "SSDP", NDPI_PROTOCOL_CATEGORY_SYSTEM_OS,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_BGP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "BGP", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 179, 2605, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_SNMP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "SNMP", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 161, 162, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_XDMCP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "XDMCP", NDPI_PROTOCOL_CATEGORY_REMOTE_ACCESS,
			  ndpi_build_default_ports(ports_a, 177, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 177, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_DANGEROUS, NDPI_PROTOCOL_SMBV1, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "SMBv1", NDPI_PROTOCOL_CATEGORY_SYSTEM_OS,
			  ndpi_build_default_ports(ports_a, 445, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_SYSLOG, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Syslog", NDPI_PROTOCOL_CATEGORY_SYSTEM_OS,
			  ndpi_build_default_ports(ports_a, 514, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 514, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_DHCP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "DHCP", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 67, 68, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_POSTGRES, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "PostgreSQL", NDPI_PROTOCOL_CATEGORY_DATABASE,
			  ndpi_build_default_ports(ports_a, 5432, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_MYSQL, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "MySQL", NDPI_PROTOCOL_CATEGORY_DATABASE,
			  ndpi_build_default_ports(ports_a, 3306, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_POTENTIALLY_DANGEROUS, NDPI_PROTOCOL_DIRECT_DOWNLOAD_LINK,
			  0 /* can_have_a_subprotocol */, no_master, no_master, "Direct_Download_Link",
			  NDPI_PROTOCOL_CATEGORY_DOWNLOAD_FT,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_POTENTIALLY_DANGEROUS, NDPI_PROTOCOL_APPLEJUICE,
			  0 /* can_have_a_subprotocol */, no_master, no_master, "AppleJuice",
			  NDPI_PROTOCOL_CATEGORY_DOWNLOAD_FT,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_POTENTIALLY_DANGEROUS, NDPI_PROTOCOL_DIRECTCONNECT,
			  0 /* can_have_a_subprotocol */, no_master, no_master, "DirectConnect",
			  NDPI_PROTOCOL_CATEGORY_DOWNLOAD_FT,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_NATS,
			  0 /* can_have_a_subprotocol */, no_master, no_master, "Nats",
			  NDPI_PROTOCOL_CATEGORY_RPC,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_SAFE, NDPI_PROTOCOL_NTOP, 0 /* can_have_a_subprotocol */, no_master,
			  no_master, "ntop", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_VMWARE, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "VMware", NDPI_PROTOCOL_CATEGORY_REMOTE_ACCESS,
			  ndpi_build_default_ports(ports_a, 903, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 902, 903, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_SAFE, NDPI_PROTOCOL_FBZERO, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "FacebookZero", NDPI_PROTOCOL_CATEGORY_SOCIAL_NETWORK,
			  ndpi_build_default_ports(ports_a, 443, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_POTENTIALLY_DANGEROUS, NDPI_PROTOCOL_KONTIKI,
			  0 /* can_have_a_subprotocol */, no_master, no_master, "Kontiki",
			  NDPI_PROTOCOL_CATEGORY_MEDIA, ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_POTENTIALLY_DANGEROUS, NDPI_PROTOCOL_OPENFT,
			  0 /* can_have_a_subprotocol */, no_master, no_master, "OpenFT",
			  NDPI_PROTOCOL_CATEGORY_DOWNLOAD_FT,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_POTENTIALLY_DANGEROUS, NDPI_PROTOCOL_FASTTRACK,
			  0 /* can_have_a_subprotocol */, no_master, no_master, "FastTrack",
			  NDPI_PROTOCOL_CATEGORY_DOWNLOAD_FT,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_POTENTIALLY_DANGEROUS, NDPI_PROTOCOL_GNUTELLA,
			  0 /* can_have_a_subprotocol */, no_master, no_master, "Gnutella",
			  NDPI_PROTOCOL_CATEGORY_DOWNLOAD_FT,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_UNSAFE, NDPI_PROTOCOL_EDONKEY, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "eDonkey", NDPI_PROTOCOL_CATEGORY_DOWNLOAD_FT,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_UNSAFE, NDPI_PROTOCOL_BITTORRENT, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "BitTorrent", NDPI_PROTOCOL_CATEGORY_DOWNLOAD_FT,
			  ndpi_build_default_ports(ports_a, 51413, 53646, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 6771, 51413, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_SKYPE, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Skype", NDPI_PROTOCOL_CATEGORY_VOIP,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_SKYPE_CALL,
			  0 /* can_have_a_subprotocol */, no_master, no_master, "SkypeCall",
			  NDPI_PROTOCOL_CATEGORY_VOIP, ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_TIKTOK, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "TikTok", NDPI_PROTOCOL_CATEGORY_SOCIAL_NETWORK,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_TEREDO, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Teredo", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 3544, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(
			  ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_WECHAT, 0 /* can_have_a_subprotocol */, no_master, /* wechat.com */
			  no_master, "WeChat", NDPI_PROTOCOL_CATEGORY_CHAT, ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_MEMCACHED, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Memcached", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 11211, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 11211, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_SMBV23, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "SMBv23", NDPI_PROTOCOL_CATEGORY_SYSTEM_OS,
			  ndpi_build_default_ports(ports_a, 445, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_UNSAFE, NDPI_PROTOCOL_MINING, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Mining", CUSTOM_CATEGORY_MINING,
			  ndpi_build_default_ports(ports_a, 8333, 30303, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_NEST_LOG_SINK,
			  0 /* can_have_a_subprotocol */, no_master, no_master, "NestLogSink",
			  NDPI_PROTOCOL_CATEGORY_CLOUD,
			  ndpi_build_default_ports(ports_a, 11095, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_MODBUS, 1 /* no subprotocol */, no_master,
			  no_master, "Modbus", NDPI_PROTOCOL_CATEGORY_NETWORK, /* Perhaps IoT in the future */
			  ndpi_build_default_ports(ports_a, 502, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_WHATSAPP_CALL,
			  0 /* can_have_a_subprotocol */, no_master, no_master, "WhatsAppCall",
			  NDPI_PROTOCOL_CATEGORY_VOIP, ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_DATASAVER, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "DataSaver", NDPI_PROTOCOL_CATEGORY_WEB /* dummy */,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_SIGNAL, 0 /* can_have_a_subprotocol */,
			  no_master, /* https://signal.org */
			  no_master, "Signal", NDPI_PROTOCOL_CATEGORY_CHAT,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_DOH_DOT, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "DoH_DoT", NDPI_PROTOCOL_CATEGORY_NETWORK /* dummy */,
			  ndpi_build_default_ports(ports_a, 853, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_FREE_205, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "FREE_205", NDPI_PROTOCOL_CATEGORY_VOIP,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_WIREGUARD, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "WireGuard", NDPI_PROTOCOL_CATEGORY_VPN,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 51820, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_PPSTREAM, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "PPStream", NDPI_PROTOCOL_CATEGORY_VIDEO,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_XBOX, 0 /* can_have_a_subprotocol */, no_master,
			  no_master, "Xbox", NDPI_PROTOCOL_CATEGORY_GAME,
			  ndpi_build_default_ports(ports_a, 3074, 3076, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 3074, 3076, 500, 3544, 4500) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_PLAYSTATION, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Playstation", NDPI_PROTOCOL_CATEGORY_GAME,
			  ndpi_build_default_ports(ports_a, 1935, 3478, 3479, 3480, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 3478, 3479, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_QQ, 0 /* can_have_a_subprotocol */, no_master,
			  no_master, "QQ", NDPI_PROTOCOL_CATEGORY_CHAT,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_RTSP, 0 /* can_have_a_subprotocol */, no_master,
			  no_master, "RTSP", NDPI_PROTOCOL_CATEGORY_MEDIA,
			  ndpi_build_default_ports(ports_a, 554, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 554, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_ICECAST, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "IceCast", NDPI_PROTOCOL_CATEGORY_MEDIA,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_PPLIVE, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "PPLive", NDPI_PROTOCOL_CATEGORY_MEDIA,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_PPSTREAM, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "PPStream", NDPI_PROTOCOL_CATEGORY_MEDIA,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_ZATTOO, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Zattoo", NDPI_PROTOCOL_CATEGORY_VIDEO,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_SHOUTCAST, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "ShoutCast", NDPI_PROTOCOL_CATEGORY_MUSIC,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_SOPCAST, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Sopcast", NDPI_PROTOCOL_CATEGORY_VIDEO,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_TVANTS, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Tvants", NDPI_PROTOCOL_CATEGORY_VIDEO,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_TVUPLAYER, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "TVUplayer", NDPI_PROTOCOL_CATEGORY_VIDEO,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_HTTP_DOWNLOAD,
			  1 /* can_have_a_subprotocol */, no_master, no_master, "HTTP_Download",
			  NDPI_PROTOCOL_CATEGORY_DOWNLOAD_FT,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_QQLIVE, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "QQLive", NDPI_PROTOCOL_CATEGORY_VIDEO,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_THUNDER, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Thunder", NDPI_PROTOCOL_CATEGORY_DOWNLOAD_FT,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_SOULSEEK, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Soulseek", NDPI_PROTOCOL_CATEGORY_DOWNLOAD_FT,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);

  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_PS_VUE, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "PS_VUE", NDPI_PROTOCOL_CATEGORY_VIDEO,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_UNSAFE, NDPI_PROTOCOL_IRC, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "IRC", NDPI_PROTOCOL_CATEGORY_CHAT,
			  ndpi_build_default_ports(ports_a, 194, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 194, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_AYIYA, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Ayiya", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 5072, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_UNENCRYPTED_JABBER,
			  0 /* can_have_a_subprotocol */, no_master, no_master, "Unencrypted_Jabber",
			  NDPI_PROTOCOL_CATEGORY_WEB, ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_OSCAR, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Oscar", NDPI_PROTOCOL_CATEGORY_CHAT,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_BATTLEFIELD, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "BattleField", NDPI_PROTOCOL_CATEGORY_GAME,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_IP_VRRP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "VRRP", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_STEAM, 0 /* can_have_a_subprotocol */, no_master,
			  no_master, "Steam", NDPI_PROTOCOL_CATEGORY_GAME,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_HALFLIFE2, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "HalfLife2", NDPI_PROTOCOL_CATEGORY_GAME,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_WORLDOFWARCRAFT, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "WorldOfWarcraft", NDPI_PROTOCOL_CATEGORY_GAME,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_POTENTIALLY_DANGEROUS, NDPI_PROTOCOL_HOTSPOT_SHIELD,
			  0 /* can_have_a_subprotocol */, no_master, no_master, "HotspotShield",
			  NDPI_PROTOCOL_CATEGORY_VPN, ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_UNSAFE, NDPI_PROTOCOL_TELNET, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Telnet", NDPI_PROTOCOL_CATEGORY_REMOTE_ACCESS,
			  ndpi_build_default_ports(ports_a, 23, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);

  custom_master[0] = NDPI_PROTOCOL_SIP, custom_master[1] = NDPI_PROTOCOL_H323;
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_STUN, 0 /* can_have_a_subprotocol */,
			  no_master, custom_master, "STUN", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 3478, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_SAFE, NDPI_PROTOCOL_IP_IPSEC, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "IPsec", NDPI_PROTOCOL_CATEGORY_VPN,
			  ndpi_build_default_ports(ports_a, 500, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 500, 4500, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_IP_GRE, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "GRE", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_IP_ICMP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "ICMP", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_IP_IGMP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "IGMP", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_IP_EGP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "EGP", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_IP_SCTP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "SCTP", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_IP_OSPF, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "OSPF", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 2604, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_IP_IP_IN_IP,
			  0 /* can_have_a_subprotocol */, no_master, no_master, "IP_in_IP",
			  NDPI_PROTOCOL_CATEGORY_NETWORK, ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_RTP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "RTP", NDPI_PROTOCOL_CATEGORY_MEDIA,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_RDP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "RDP", NDPI_PROTOCOL_CATEGORY_REMOTE_ACCESS,
			  ndpi_build_default_ports(ports_a, 3389, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 3389, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_VNC, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "VNC", NDPI_PROTOCOL_CATEGORY_REMOTE_ACCESS,
			  ndpi_build_default_ports(ports_a, 5900, 5901, 5800, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_PCANYWHERE,
			  0 /* can_have_a_subprotocol */, no_master, no_master, "PcAnywhere",
			  NDPI_PROTOCOL_CATEGORY_REMOTE_ACCESS,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_ZOOM, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Zoom", NDPI_PROTOCOL_CATEGORY_VIDEO,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_WHATSAPP_FILES,
			  0 /* can_have_a_subprotocol */, no_master, no_master, "WhatsAppFiles",
			  NDPI_PROTOCOL_CATEGORY_DOWNLOAD_FT,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_WHATSAPP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "WhatsApp", NDPI_PROTOCOL_CATEGORY_CHAT,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);

  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_SAFE, NDPI_PROTOCOL_TLS, 1 /* can_have_a_subprotocol */, no_master,
			  no_master, "TLS", NDPI_PROTOCOL_CATEGORY_WEB,
			  ndpi_build_default_ports(ports_a, 443, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_SSH, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "SSH", NDPI_PROTOCOL_CATEGORY_REMOTE_ACCESS,
			  ndpi_build_default_ports(ports_a, 22, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_USENET, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Usenet", NDPI_PROTOCOL_CATEGORY_WEB,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_MGCP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "MGCP", NDPI_PROTOCOL_CATEGORY_VOIP,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_IAX, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "IAX", NDPI_PROTOCOL_CATEGORY_VOIP,
			  ndpi_build_default_ports(ports_a, 4569, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 4569, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_AFP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "AFP", NDPI_PROTOCOL_CATEGORY_DATA_TRANSFER,
			  ndpi_build_default_ports(ports_a, 548, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 548, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_HULU, 0 /* can_have_a_subprotocol */, no_master,
			  no_master, "Hulu", NDPI_PROTOCOL_CATEGORY_STREAMING,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_CHECKMK, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "CHECKMK", NDPI_PROTOCOL_CATEGORY_DATA_TRANSFER,
			  ndpi_build_default_ports(ports_a, 6556, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_POTENTIALLY_DANGEROUS, NDPI_PROTOCOL_STEALTHNET,
			  0 /* can_have_a_subprotocol */, no_master, no_master, "Stealthnet",
			  NDPI_PROTOCOL_CATEGORY_DOWNLOAD_FT,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_AIMINI, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Aimini", NDPI_PROTOCOL_CATEGORY_DOWNLOAD_FT,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_SIP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "SIP", NDPI_PROTOCOL_CATEGORY_VOIP,
			  ndpi_build_default_ports(ports_a, 5060, 5061, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 5060, 5061, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_TRUPHONE, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "TruPhone", NDPI_PROTOCOL_CATEGORY_VOIP,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_IP_ICMPV6, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "ICMPV6", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_DHCPV6, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "DHCPV6", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_ARMAGETRON, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Armagetron", NDPI_PROTOCOL_CATEGORY_GAME,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_CROSSFIRE, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Crossfire", NDPI_PROTOCOL_CATEGORY_RPC,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_DOFUS, 0 /* can_have_a_subprotocol */, no_master,
			  no_master, "Dofus", NDPI_PROTOCOL_CATEGORY_GAME,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_FIESTA, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Fiesta", NDPI_PROTOCOL_CATEGORY_GAME,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_FLORENSIA, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Florensia", NDPI_PROTOCOL_CATEGORY_GAME,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_GUILDWARS, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Guildwars", NDPI_PROTOCOL_CATEGORY_GAME,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_HTTP_ACTIVESYNC,
			  1 /* can_have_a_subprotocol */, no_master, no_master, "HTTP_ActiveSync",
			  NDPI_PROTOCOL_CATEGORY_CLOUD, ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_KERBEROS, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Kerberos", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 88, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 88, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_LDAP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "LDAP", NDPI_PROTOCOL_CATEGORY_SYSTEM_OS,
			  ndpi_build_default_ports(ports_a, 389, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 389, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_MAPLESTORY, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "MapleStory", NDPI_PROTOCOL_CATEGORY_GAME,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_MSSQL_TDS, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "MsSQL-TDS", NDPI_PROTOCOL_CATEGORY_DATABASE,
			  ndpi_build_default_ports(ports_a, 1433, 1434, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_PPTP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "PPTP", NDPI_PROTOCOL_CATEGORY_VPN,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_WARCRAFT3, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Warcraft3", NDPI_PROTOCOL_CATEGORY_GAME,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_WORLD_OF_KUNG_FU, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "WorldOfKungFu", NDPI_PROTOCOL_CATEGORY_GAME,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_DCERPC, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "DCE_RPC", NDPI_PROTOCOL_CATEGORY_RPC,
			  ndpi_build_default_ports(ports_a, 135, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_NETFLOW, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "NetFlow", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 2055, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_SFLOW, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "sFlow", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 6343, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_HTTP_CONNECT,
			  1 /* can_have_a_subprotocol */, no_master, no_master, "HTTP_Connect",
			  NDPI_PROTOCOL_CATEGORY_WEB, ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_HTTP_PROXY,
			  1 /* can_have_a_subprotocol */, no_master, no_master, "HTTP_Proxy",
			  NDPI_PROTOCOL_CATEGORY_WEB,
			  ndpi_build_default_ports(ports_a, 8080, 3128, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_CITRIX, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Citrix", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 1494, 2598, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_WEBEX, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Webex", NDPI_PROTOCOL_CATEGORY_VOIP,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_RADIUS, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Radius", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 1812, 1813, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 1812, 1813, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_TEAMVIEWER,
			  0 /* can_have_a_subprotocol */, no_master, no_master, "TeamViewer",
			  NDPI_PROTOCOL_CATEGORY_REMOTE_ACCESS,
			  ndpi_build_default_ports(ports_a, 5938, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 5938, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_LOTUS_NOTES,
			  0 /* can_have_a_subprotocol */, no_master, no_master, "LotusNotes",
			  NDPI_PROTOCOL_CATEGORY_COLLABORATIVE,
			  ndpi_build_default_ports(ports_a, 1352, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(
			  ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_SAP, 0 /* can_have_a_subprotocol */, no_master, no_master,
			  "SAP", NDPI_PROTOCOL_CATEGORY_NETWORK, ndpi_build_default_ports(ports_a, 3201, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */); /* Missing dissector: port based only */
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_GTP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "GTP", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 2152, 2123, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_UPNP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "UPnP", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 1780, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 1900, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_TELEGRAM, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Telegram", NDPI_PROTOCOL_CATEGORY_CHAT,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_QUIC, 1 /* can_have_a_subprotocol */,
			  no_master, no_master, "QUIC", NDPI_PROTOCOL_CATEGORY_WEB,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 443, 80, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_DIAMETER, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Diameter", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 3868, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_APPLE_PUSH,
			  0 /* can_have_a_subprotocol */, no_master, no_master, "ApplePush",
			  NDPI_PROTOCOL_CATEGORY_CLOUD, ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_DROPBOX, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Dropbox", NDPI_PROTOCOL_CATEGORY_CLOUD,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 17500, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_SPOTIFY, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Spotify", NDPI_PROTOCOL_CATEGORY_MUSIC,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_MESSENGER, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Messenger", NDPI_PROTOCOL_CATEGORY_VOIP,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_LISP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "LISP", NDPI_PROTOCOL_CATEGORY_CLOUD,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 4342, 4341, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_EAQ, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "EAQ", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 6000, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_KAKAOTALK_VOICE,
			  0 /* can_have_a_subprotocol */, no_master, no_master, "KakaoTalk_Voice",
			  NDPI_PROTOCOL_CATEGORY_VOIP, ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_MPEGTS, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "MPEG_TS", NDPI_PROTOCOL_CATEGORY_MEDIA,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  /* http://en.wikipedia.org/wiki/Link-local_Multicast_Name_Resolution */
  ndpi_set_proto_defaults(
			  ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_LLMNR, 0 /* can_have_a_subprotocol */, no_master, no_master,
			  "LLMNR", NDPI_PROTOCOL_CATEGORY_NETWORK, ndpi_build_default_ports(ports_a, 5355, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 5355, 0, 0, 0, 0) /* UDP */); /* Missing dissector: port based only */
  ndpi_set_proto_defaults(
			  ndpi_str, NDPI_PROTOCOL_POTENTIALLY_DANGEROUS, NDPI_PROTOCOL_REMOTE_SCAN, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "RemoteScan", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 6077, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 6078, 0, 0, 0, 0) /* UDP */); /* Missing dissector: port based only */

  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_H323, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "H323", NDPI_PROTOCOL_CATEGORY_VOIP,
			  ndpi_build_default_ports(ports_a, 1719, 1720, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 1719, 1720, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_OPENVPN, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "OpenVPN", NDPI_PROTOCOL_CATEGORY_VPN,
			  ndpi_build_default_ports(ports_a, 1194, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 1194, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_NOE, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "NOE", NDPI_PROTOCOL_CATEGORY_VOIP,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_CISCOVPN, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "CiscoVPN", NDPI_PROTOCOL_CATEGORY_VPN,
			  ndpi_build_default_ports(ports_a, 10000, 8008, 8009, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 10000, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_TEAMSPEAK, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "TeamSpeak", NDPI_PROTOCOL_CATEGORY_VOIP,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_SKINNY, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "CiscoSkinny", NDPI_PROTOCOL_CATEGORY_VOIP,
			  ndpi_build_default_ports(ports_a, 2000, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_RTCP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "RTCP", NDPI_PROTOCOL_CATEGORY_VOIP,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_RSYNC, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "RSYNC", NDPI_PROTOCOL_CATEGORY_DATA_TRANSFER,
			  ndpi_build_default_ports(ports_a, 873, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_ORACLE, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Oracle", NDPI_PROTOCOL_CATEGORY_DATABASE,
			  ndpi_build_default_ports(ports_a, 1521, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_CORBA, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Corba", NDPI_PROTOCOL_CATEGORY_RPC,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_UBUNTUONE, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "UbuntuONE", NDPI_PROTOCOL_CATEGORY_CLOUD,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_WHOIS_DAS, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Whois-DAS", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 43, 4343, 0, 0, 0), /* TCP */
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0));    /* UDP */
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_COLLECTD, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Collectd", NDPI_PROTOCOL_CATEGORY_SYSTEM_OS,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0),      /* TCP */
			  ndpi_build_default_ports(ports_b, 25826, 0, 0, 0, 0)); /* UDP */
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_SOCKS, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "SOCKS", NDPI_PROTOCOL_CATEGORY_WEB,
			  ndpi_build_default_ports(ports_a, 1080, 0, 0, 0, 0),  /* TCP */
			  ndpi_build_default_ports(ports_b, 1080, 0, 0, 0, 0)); /* UDP */
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_TFTP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "TFTP", NDPI_PROTOCOL_CATEGORY_DATA_TRANSFER,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0),   /* TCP */
			  ndpi_build_default_ports(ports_b, 69, 0, 0, 0, 0)); /* UDP */
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_RTMP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "RTMP", NDPI_PROTOCOL_CATEGORY_MEDIA,
			  ndpi_build_default_ports(ports_a, 1935, 0, 0, 0, 0), /* TCP */
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0));   /* UDP */
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_PANDO, 0 /* can_have_a_subprotocol */, no_master,
			  no_master, "Pando_Media_Booster", NDPI_PROTOCOL_CATEGORY_WEB,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0),  /* TCP */
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0)); /* UDP */
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_MEGACO, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Megaco", NDPI_PROTOCOL_CATEGORY_VOIP,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0),     /* TCP */
			  ndpi_build_default_ports(ports_b, 2944, 0, 0, 0, 0)); /* UDP */
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_REDIS, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Redis", NDPI_PROTOCOL_CATEGORY_DATABASE,
			  ndpi_build_default_ports(ports_a, 6379, 0, 0, 0, 0), /* TCP */
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0));   /* UDP */
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_ZMQ, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "ZeroMQ", NDPI_PROTOCOL_CATEGORY_RPC,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0),  /* TCP */
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0)); /* UDP */
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_VHUA, 0 /* can_have_a_subprotocol */, no_master,
			  no_master, "VHUA", NDPI_PROTOCOL_CATEGORY_VOIP,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0),      /* TCP */
			  ndpi_build_default_ports(ports_b, 58267, 0, 0, 0, 0)); /* UDP */
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_STARCRAFT, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Starcraft", NDPI_PROTOCOL_CATEGORY_GAME,
			  ndpi_build_default_ports(ports_a, 1119, 0, 0, 0, 0),  /* TCP */
			  ndpi_build_default_ports(ports_b, 1119, 0, 0, 0, 0)); /* UDP */
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_SAFE, NDPI_PROTOCOL_UBNTAC2, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "UBNTAC2", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0),      /* TCP */
			  ndpi_build_default_ports(ports_b, 10001, 0, 0, 0, 0)); /* UDP */
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_VIBER, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Viber", NDPI_PROTOCOL_CATEGORY_VOIP,
			  ndpi_build_default_ports(ports_a, 7985, 5242, 5243, 4244, 0),     /* TCP */
			  ndpi_build_default_ports(ports_b, 7985, 7987, 5242, 5243, 4244)); /* UDP */
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_SAFE, NDPI_PROTOCOL_COAP, 0 /* can_have_a_subprotocol */, no_master,
			  no_master, "COAP", NDPI_PROTOCOL_CATEGORY_RPC,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0),        /* TCP */
			  ndpi_build_default_ports(ports_b, 5683, 5684, 0, 0, 0)); /* UDP */
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_MQTT, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "MQTT", NDPI_PROTOCOL_CATEGORY_RPC,
			  ndpi_build_default_ports(ports_a, 1883, 8883, 0, 0, 0), /* TCP */
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0));      /* UDP */
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_SOMEIP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "SOMEIP", NDPI_PROTOCOL_CATEGORY_RPC,
			  ndpi_build_default_ports(ports_a, 30491, 30501, 0, 0, 0),      /* TCP */
			  ndpi_build_default_ports(ports_b, 30491, 30501, 30490, 0, 0)); /* UDP */
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_RX, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "RX", NDPI_PROTOCOL_CATEGORY_RPC,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0),  /* TCP */
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0)); /* UDP */
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_SAFE, NDPI_PROTOCOL_GIT, 0 /* can_have_a_subprotocol */, no_master,
			  no_master, "Git", NDPI_PROTOCOL_CATEGORY_COLLABORATIVE,
			  ndpi_build_default_ports(ports_a, 9418, 0, 0, 0, 0), /* TCP */
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0));   /* UDP */
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_DRDA, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "DRDA", NDPI_PROTOCOL_CATEGORY_DATABASE,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0),  /* TCP */
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0)); /* UDP */
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_HANGOUT_DUO,
			  0 /* can_have_a_subprotocol */, no_master, no_master, "GoogleHangoutDuo",
			  NDPI_PROTOCOL_CATEGORY_VOIP, ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_BJNP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "BJNP", NDPI_PROTOCOL_CATEGORY_SYSTEM_OS,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 8612, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_SMPP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "SMPP", NDPI_PROTOCOL_CATEGORY_DOWNLOAD_FT,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0),  /* TCP */
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0)); /* UDP */
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_SAFE, NDPI_PROTOCOL_OOKLA, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Ookla", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0),  /* TCP */
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0)); /* UDP */
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_AMQP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "AMQP", NDPI_PROTOCOL_CATEGORY_RPC,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0),  /* TCP */
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0)); /* UDP */
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_SAFE, NDPI_PROTOCOL_DNSCRYPT, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "DNScrypt", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0),  /* TCP */
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0)); /* UDP */
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_TINC, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "TINC", NDPI_PROTOCOL_CATEGORY_VPN,
			  ndpi_build_default_ports(ports_a, 655, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 655, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_SAFE, NDPI_PROTOCOL_FIX, 0 /* can_have_a_subprotocol */, no_master,
			  no_master, "FIX", NDPI_PROTOCOL_CATEGORY_RPC,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_NINTENDO, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "Nintendo", NDPI_PROTOCOL_CATEGORY_GAME,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_FUN, NDPI_PROTOCOL_CSGO, 0 /* can_have_a_subprotocol */, no_master,
			  no_master, "CSGO", NDPI_PROTOCOL_CATEGORY_GAME,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_AJP, 0 /* can_have_a_subprotocol */,
			  no_master, no_master, "AJP", NDPI_PROTOCOL_CATEGORY_WEB,
			  ndpi_build_default_ports(ports_a, 8009, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_TARGUS_GETDATA,
			  0 /* can_have_a_subprotocol */, no_master, no_master, "Targus Dataspeed",
			  NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 5001, 5201, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 5001, 5201, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_AMAZON_VIDEO,
			  0 /* can_have_a_subprotocol */, no_master, no_master, "AmazonVideo",
			  NDPI_PROTOCOL_CATEGORY_CLOUD, ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_DNP3, 1 /* no subprotocol */, no_master,
			  no_master, "DNP3", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 20000, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_IEC60870, 1 /* no subprotocol */,
			  no_master, no_master, "IEC60870",
			  NDPI_PROTOCOL_CATEGORY_NETWORK, /* Perhaps IoT in the future */
			  ndpi_build_default_ports(ports_a, 2404, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_BLOOMBERG, 1 /* no subprotocol */,
			  no_master, no_master, "Bloomberg", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_CAPWAP, 1 /* no subprotocol */, no_master,
			  no_master, "CAPWAP", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 5246, 5247, 0, 0, 0) /* UDP */
			  );
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_ZABBIX, 1 /* no subprotocol */, no_master,
			  no_master, "Zabbix", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 10050, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */
			  );
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_S7COMM, 1 /* no subprotocol */, no_master,
			  no_master, "s7comm", NDPI_PROTOCOL_CATEGORY_NETWORK,
			  ndpi_build_default_ports(ports_a, 102, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_SAFE, NDPI_PROTOCOL_MSTEAMS, 1 /* no subprotocol */, no_master,
			  no_master, "Teams", NDPI_PROTOCOL_CATEGORY_COLLABORATIVE,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */
			  );
  ndpi_set_proto_defaults(ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, NDPI_PROTOCOL_WEBSOCKET,
			  1 /* can_have_a_subprotocol */, no_master,
			  no_master, "WebSocket", NDPI_PROTOCOL_CATEGORY_WEB,
			  ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			  ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);

#ifdef CUSTOM_NDPI_PROTOCOLS
#include "../../../nDPI-custom/custom_ndpi_main.c"
#endif

  /* calling function for host and content matched protocols */
  init_string_based_protocols(ndpi_str);

  ndpi_validate_protocol_initialization(ndpi_str);
}