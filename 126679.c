static size_t p_seek(void *fp, size_t offset) {
#if (defined(_FILE_OFFSET_BITS) && _FILE_OFFSET_BITS == 64) ||  \
    (defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L) || \
    (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 600)
  fseeko((FILE *) fp, (off_t) offset, SEEK_SET);
#else
  fseek((FILE *) fp, (long) offset, SEEK_SET);
#endif
  return (size_t) ftell((FILE *) fp);
}