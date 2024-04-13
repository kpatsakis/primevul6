void ndpi_self_check_host_match() {
  u_int32_t i, j;

  for (i = 0; host_match[i].string_to_match != NULL; i++) {
    for (j = 0; host_match[j].string_to_match != NULL; j++) {
      if((i != j) && (strcmp(host_match[i].string_to_match, host_match[j].string_to_match) == 0)) {
	printf("[INTERNAL ERROR]: Duplicate string detected '%s' [id: %u, id %u]\n",
	       host_match[i].string_to_match, i, j);
	printf("\nPlease fix host_match[] in ndpi_content_match.c.inc\n");
	exit(0);
      }
    }
  }
}