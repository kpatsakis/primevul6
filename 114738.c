u_int8_t ndpi_extra_dissection_possible(struct ndpi_detection_module_struct *ndpi_str,
					struct ndpi_flow_struct *flow) {
  u_int16_t proto =
    flow->detected_protocol_stack[1] ? flow->detected_protocol_stack[1] : flow->detected_protocol_stack[0];

#if 0
  printf("[DEBUG] %s(%u.%u): %u\n", __FUNCTION__,
	 flow->detected_protocol_stack[0],
	 flow->detected_protocol_stack[1],
	 proto);
#endif

  switch (proto) {
  case NDPI_PROTOCOL_TLS:
    if(!flow->l4.tcp.tls.certificate_processed)
      return(1); /* TODO: add check for TLS 1.3 */
    break;

  case NDPI_PROTOCOL_HTTP:
    if((flow->host_server_name[0] == '\0') || (flow->http.response_status_code == 0))
      return(1);
    break;

  case NDPI_PROTOCOL_DNS:
    if(flow->protos.dns.num_answers == 0)
      return(1);
    break;

  case NDPI_PROTOCOL_FTP_CONTROL:
  case NDPI_PROTOCOL_MAIL_POP:
  case NDPI_PROTOCOL_MAIL_IMAP:
  case NDPI_PROTOCOL_MAIL_SMTP:
    if(flow->protos.ftp_imap_pop_smtp.password[0] == '\0')
      return(1);
    break;

  case NDPI_PROTOCOL_SSH:
    if((flow->protos.ssh.hassh_client[0] == '\0') || (flow->protos.ssh.hassh_server[0] == '\0'))
      return(1);
    break;

  case NDPI_PROTOCOL_TELNET:
    if(!flow->protos.telnet.password_detected)
      return(1);
    break;
  }

  return(0);
}