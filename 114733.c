char *ndpi_strncasestr(const char *s, const char *find, size_t slen) {
  char c;
  size_t len;

  if((c = *find++) != '\0') {
    len = strlen(find);
    do {
      char sc;

      do {
	if(slen-- < 1 || (sc = *s++) == '\0')
	  return(NULL);
      } while (sc != c);

      if(len > slen)
	return(NULL);
    } while (strncasecmp(s, find, len) != 0);

    s--;
  }
  return((char *) s);
}