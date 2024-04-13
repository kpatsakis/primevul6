static size_t Curl_strnlen(const char *string, size_t maxlen)
{
  const char *end = memchr(string, '\0', maxlen);
  return end ? (size_t) (end - string) : maxlen;
}