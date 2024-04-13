static void ndpi_ssh_zap_cr(char *str, int len) {
  len--;

  while(len > 0) {
    if((str[len] == '\n') || (str[len] == '\r')) {
      str[len] = '\0';
      len--;
    } else
      break;
  }
}