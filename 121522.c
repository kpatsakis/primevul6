static void ndpi_search_ssh_tcp(struct ndpi_detection_module_struct *ndpi_struct, struct ndpi_flow_struct *flow) {
  struct ndpi_packet_struct *packet = &flow->packet;

#ifdef SSH_DEBUG
  printf("[SSH] %s()\n", __FUNCTION__);
#endif

  if(flow->l4.tcp.ssh_stage == 0) {
    if(packet->payload_packet_len > 7 && packet->payload_packet_len < 100
	&& memcmp(packet->payload, "SSH-", 4) == 0) {
      int len = ndpi_min(sizeof(flow->protos.ssh.client_signature)-1, packet->payload_packet_len);
      
      strncpy(flow->protos.ssh.client_signature, (const char *)packet->payload, len);
      flow->protos.ssh.client_signature[len] = '\0';
      ndpi_ssh_zap_cr(flow->protos.ssh.client_signature, len);
      
#ifdef SSH_DEBUG
      printf("[SSH] [client_signature: %s]\n", flow->protos.ssh.client_signature);
#endif      
      
      NDPI_LOG_DBG2(ndpi_struct, "ssh stage 0 passed\n");
      flow->l4.tcp.ssh_stage = 1 + packet->packet_direction;
      ndpi_int_ssh_add_connection(ndpi_struct, flow);
      return;
    }
  } else if(flow->l4.tcp.ssh_stage == (2 - packet->packet_direction)) {
    if(packet->payload_packet_len > 7 && packet->payload_packet_len < 500
	&& memcmp(packet->payload, "SSH-", 4) == 0) {
      int len = ndpi_min(sizeof(flow->protos.ssh.server_signature)-1, packet->payload_packet_len);
      
      strncpy(flow->protos.ssh.server_signature, (const char *)packet->payload, len);
      flow->protos.ssh.server_signature[len] = '\0';
      ndpi_ssh_zap_cr(flow->protos.ssh.server_signature, len);
      
#ifdef SSH_DEBUG
      printf("[SSH] [server_signature: %s]\n", flow->protos.ssh.server_signature);
#endif
      
      NDPI_LOG_DBG2(ndpi_struct, "ssh stage 1 passed\n");
      flow->guessed_host_protocol_id = flow->guessed_protocol_id = NDPI_PROTOCOL_SSH;
      
#ifdef SSH_DEBUG
      printf("[SSH] [completed stage: %u]\n", flow->l4.tcp.ssh_stage);
#endif

      flow->l4.tcp.ssh_stage = 3;
      return;
    }
  } else if(packet->payload_packet_len > 5) {
    u_int8_t msgcode = *(packet->payload + 5);
    ndpi_MD5_CTX ctx;
    
    if(msgcode == 20 /* key exchange init */) {
      char *hassh_buf = ndpi_calloc(packet->payload_packet_len, sizeof(char));
      u_int i, len;

#ifdef SSH_DEBUG
      printf("[SSH] [stage: %u][msg: %u][direction: %u][key exchange init]\n", flow->l4.tcp.ssh_stage, msgcode, packet->packet_direction);
#endif

      if(hassh_buf) {
	if(packet->packet_direction == 0 /* client */) {
	  u_char fingerprint_client[16];

	  len = concat_hash_string(packet, hassh_buf, 1 /* client */);

	  ndpi_MD5Init(&ctx);
	  ndpi_MD5Update(&ctx, (const unsigned char *)hassh_buf, len);
	  ndpi_MD5Final(fingerprint_client, &ctx);

#ifdef SSH_DEBUG
	  {
	    printf("[SSH] [client][%s][", hassh_buf);
	    for(i=0; i<16; i++) printf("%02X", fingerprint_client[i]);
	    printf("]\n");
	  }
#endif
	  for(i=0; i<16; i++) sprintf(&flow->protos.ssh.hassh_client[i*2], "%02X", fingerprint_client[i] & 0xFF);
	  flow->protos.ssh.hassh_client[32] = '\0';
	} else {
	  u_char fingerprint_server[16];

	  len = concat_hash_string(packet, hassh_buf, 0 /* server */);

	  ndpi_MD5Init(&ctx);
	  ndpi_MD5Update(&ctx, (const unsigned char *)hassh_buf, len);
	  ndpi_MD5Final(fingerprint_server, &ctx);

#ifdef SSH_DEBUG
	  {
	    printf("[SSH] [server][%s][", hassh_buf);
	    for(i=0; i<16; i++) printf("%02X", fingerprint_server[i]);
	    printf("]\n");
	  }
#endif

	  for(i=0; i<16; i++) sprintf(&flow->protos.ssh.hassh_server[i*2], "%02X", fingerprint_server[i] & 0xFF);
	  flow->protos.ssh.hassh_server[32] = '\0';
	}

	ndpi_free(hassh_buf);
      }

      ndpi_int_ssh_add_connection(ndpi_struct, flow);
    }

    if((flow->protos.ssh.hassh_client[0] != '\0') && (flow->protos.ssh.hassh_server[0] != '\0')) {
#ifdef SSH_DEBUG
      printf("[SSH] Dissection completed\n");
#endif
      flow->extra_packets_func = NULL; /* We're good now */
    }

    return;
  }

#ifdef SSH_DEBUG
  printf("[SSH] Excluding SSH");
#endif

  NDPI_LOG_DBG(ndpi_struct, "excluding ssh at stage %d\n", flow->l4.tcp.ssh_stage);
  NDPI_ADD_PROTOCOL_TO_BITMASK(flow->excluded_protocol_bitmask, NDPI_PROTOCOL_SSH);
}