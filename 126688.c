static const char *skip(const char *s, const char *e, const char *d,
                        struct mg_str *v) {
  v->ptr = s;
  while (s < e && *s != '\n' && strchr(d, *s) == NULL) s++;
  v->len = (size_t) (s - v->ptr);
  while (s < e && strchr(d, *s) != NULL) s++;
  return s;
}