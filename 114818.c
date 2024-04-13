u_int16_t ndpi_check_for_email_address(struct ndpi_detection_module_struct *ndpi_str, struct ndpi_flow_struct *flow,
                                       u_int16_t counter) {
  struct ndpi_packet_struct *packet = &flow->packet;

  NDPI_LOG_DBG2(ndpi_str, "called ndpi_check_for_email_address\n");

  if(packet->payload_packet_len > counter && ((packet->payload[counter] >= 'a' && packet->payload[counter] <= 'z') ||
					      (packet->payload[counter] >= 'A' && packet->payload[counter] <= 'Z') ||
					      (packet->payload[counter] >= '0' && packet->payload[counter] <= '9') ||
					      packet->payload[counter] == '-' || packet->payload[counter] == '_')) {
    NDPI_LOG_DBG2(ndpi_str, "first letter\n");
    counter++;
    while (packet->payload_packet_len > counter &&
	   ((packet->payload[counter] >= 'a' && packet->payload[counter] <= 'z') ||
	    (packet->payload[counter] >= 'A' && packet->payload[counter] <= 'Z') ||
	    (packet->payload[counter] >= '0' && packet->payload[counter] <= '9') ||
	    packet->payload[counter] == '-' || packet->payload[counter] == '_' ||
	    packet->payload[counter] == '.')) {
      NDPI_LOG_DBG2(ndpi_str, "further letter\n");
      counter++;
      if(packet->payload_packet_len > counter && packet->payload[counter] == '@') {
	NDPI_LOG_DBG2(ndpi_str, "@\n");
	counter++;
	while (packet->payload_packet_len > counter &&
	       ((packet->payload[counter] >= 'a' && packet->payload[counter] <= 'z') ||
		(packet->payload[counter] >= 'A' && packet->payload[counter] <= 'Z') ||
		(packet->payload[counter] >= '0' && packet->payload[counter] <= '9') ||
		packet->payload[counter] == '-' || packet->payload[counter] == '_')) {
	  NDPI_LOG_DBG2(ndpi_str, "letter\n");
	  counter++;
	  if(packet->payload_packet_len > counter && packet->payload[counter] == '.') {
	    NDPI_LOG_DBG2(ndpi_str, ".\n");
	    counter++;
	    if(packet->payload_packet_len > counter + 1 &&
	       ((packet->payload[counter] >= 'a' && packet->payload[counter] <= 'z') &&
		(packet->payload[counter + 1] >= 'a' && packet->payload[counter + 1] <= 'z'))) {
	      NDPI_LOG_DBG2(ndpi_str, "two letters\n");
	      counter += 2;
	      if(packet->payload_packet_len > counter &&
		 (packet->payload[counter] == ' ' || packet->payload[counter] == ';')) {
		NDPI_LOG_DBG2(ndpi_str, "whitespace1\n");
		return(counter);
	      } else if(packet->payload_packet_len > counter && packet->payload[counter] >= 'a' &&
			packet->payload[counter] <= 'z') {
		NDPI_LOG_DBG2(ndpi_str, "one letter\n");
		counter++;
		if(packet->payload_packet_len > counter &&
		   (packet->payload[counter] == ' ' || packet->payload[counter] == ';')) {
		  NDPI_LOG_DBG2(ndpi_str, "whitespace2\n");
		  return(counter);
		} else if(packet->payload_packet_len > counter && packet->payload[counter] >= 'a' &&
			  packet->payload[counter] <= 'z') {
		  counter++;
		  if(packet->payload_packet_len > counter &&
		     (packet->payload[counter] == ' ' || packet->payload[counter] == ';')) {
		    NDPI_LOG_DBG2(ndpi_str, "whitespace3\n");
		    return(counter);
		  } else {
		    return(0);
		  }
		} else {
		  return(0);
		}
	      } else {
		return(0);
	      }
	    } else {
	      return(0);
	    }
	  }
	}
	return(0);
      }
    }
  }
  return(0);
}