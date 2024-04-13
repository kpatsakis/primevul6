int ndpi_load_ipv4_ptree(struct ndpi_detection_module_struct *ndpi_str,
			 const char *path, u_int16_t protocol_id) {
  char buffer[128], *line, *addr, *cidr, *saveptr;
  FILE *fd;
  int len;
  u_int num_loaded = 0;

  fd = fopen(path, "r");

  if(fd == NULL) {
    NDPI_LOG_ERR(ndpi_str, "Unable to open file %s [%s]\n", path, strerror(errno));
    return(-1);
  }

  while (1) {
    line = fgets(buffer, sizeof(buffer), fd);

    if(line == NULL)
      break;

    len = strlen(line);

    if((len <= 1) || (line[0] == '#'))
      continue;

    line[len - 1] = '\0';
    addr = strtok_r(line, "/", &saveptr);

    if(addr) {
      struct in_addr pin;
      patricia_node_t *node;

      cidr = strtok_r(NULL, "\n", &saveptr);

      pin.s_addr = inet_addr(addr);
      if((node = add_to_ptree(ndpi_str->protocols_ptree, AF_INET, &pin, cidr ? atoi(cidr) : 32 /* bits */)) != NULL) {
	node->value.uv.user_value = protocol_id, node->value.uv.additional_user_value = 0 /* port */;
	num_loaded++;
      }
    }
  }

  fclose(fd);
  return(num_loaded);
}