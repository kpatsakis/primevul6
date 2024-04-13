static unsigned adler32(const unsigned char* data, unsigned len)
{
  return update_adler32(1L, data, len);
}