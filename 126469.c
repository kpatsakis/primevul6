char *mg_http_etag(char *buf, size_t len, size_t size, time_t mtime) {
  snprintf(buf, len, "\"%lx." MG_INT64_FMT "\"", (unsigned long) mtime,
           (int64_t) size);
  return buf;
}