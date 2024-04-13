static curl_off_t vms_realfilesize(const char *name,
                                   const struct_stat *stat_buf)
{
  char buffer[8192];
  curl_off_t count;
  int ret_stat;
  FILE * file;

  /* !checksrc! disable FOPENMODE 1 */
  file = fopen(name, "r"); /* VMS */
  if(!file) {
    return 0;
  }
  count = 0;
  ret_stat = 1;
  while(ret_stat > 0) {
    ret_stat = fread(buffer, 1, sizeof(buffer), file);
    if(ret_stat)
      count += ret_stat;
  }
  fclose(file);

  return count;
}