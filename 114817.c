int ndpi_load_categories_file(struct ndpi_detection_module_struct *ndpi_str, const char *path) {
  char buffer[512], *line, *name, *category, *saveptr;
  FILE *fd;
  int len, num = 0;

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
    name = strtok_r(line, "\t", &saveptr);

    if(name) {
      category = strtok_r(NULL, "\t", &saveptr);

      if(category) {
	int rc = ndpi_load_category(ndpi_str, name, (ndpi_protocol_category_t) atoi(category));

	if(rc >= 0)
	  num++;
      }
    }
  }

  fclose(fd);
  ndpi_enable_loaded_categories(ndpi_str);

  return(num);
}