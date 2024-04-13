void ndpi_parse_packet_line_info_any(struct ndpi_detection_module_struct *ndpi_str, struct ndpi_flow_struct *flow) {
  struct ndpi_packet_struct *packet = &flow->packet;
  u_int32_t a;
  u_int16_t end = packet->payload_packet_len;

  if(packet->packet_lines_parsed_complete != 0)
    return;

  packet->packet_lines_parsed_complete = 1;
  packet->parsed_lines = 0;

  if(packet->payload_packet_len == 0)
    return;

  packet->line[packet->parsed_lines].ptr = packet->payload;
  packet->line[packet->parsed_lines].len = 0;

  for (a = 0; a < end; a++) {
    if(packet->payload[a] == 0x0a) {
      packet->line[packet->parsed_lines].len = (u_int16_t)(
							   ((unsigned long) &packet->payload[a]) - ((unsigned long) packet->line[packet->parsed_lines].ptr));

      if(a > 0 && packet->payload[a - 1] == 0x0d)
	packet->line[packet->parsed_lines].len--;

      if(packet->parsed_lines >= (NDPI_MAX_PARSE_LINES_PER_PACKET - 1))
	break;

      packet->parsed_lines++;
      packet->line[packet->parsed_lines].ptr = &packet->payload[a + 1];
      packet->line[packet->parsed_lines].len = 0;

      if((a + 1) >= packet->payload_packet_len)
	break;

      //a++;
    }
  }
}