int ndpi_load_protocols_file(struct ndpi_detection_module_struct *ndpi_str, const char *path) {
  FILE *fd;
  char *buffer, *old_buffer;
  int chunk_len = 512, buffer_len = chunk_len, old_buffer_len;
  int i, rc = -1;

  fd = fopen(path, "r");

  if(fd == NULL) {
    NDPI_LOG_ERR(ndpi_str, "Unable to open file %s [%s]\n", path, strerror(errno));
    goto error;
  }

  buffer = ndpi_malloc(buffer_len);

  if(buffer == NULL) {
    NDPI_LOG_ERR(ndpi_str, "Memory allocation failure\n");
    goto close_fd;
  }

  while (1) {
    char *line = buffer;
    int line_len = buffer_len;

    while ((line = fgets(line, line_len, fd)) != NULL && line[strlen(line) - 1] != '\n') {
      i = strlen(line);
      old_buffer = buffer;
      old_buffer_len = buffer_len;
      buffer_len += chunk_len;

      buffer = ndpi_realloc(old_buffer, old_buffer_len, buffer_len);

      if(buffer == NULL) {
	NDPI_LOG_ERR(ndpi_str, "Memory allocation failure\n");
	ndpi_free(old_buffer);
	goto close_fd;
      }

      line = &buffer[i];
      line_len = chunk_len;
    }

    if(!line) /* safety check */
      break;

    i = strlen(buffer);
    if((i <= 1) || (buffer[0] == '#'))
      continue;
    else
      buffer[i - 1] = '\0';

    ndpi_handle_rule(ndpi_str, buffer, 1);
  }

  rc = 0;

  ndpi_free(buffer);

 close_fd:
  fclose(fd);

 error:
  return(rc);
}