int64_t OutputFile::writeImpl(const char *buffer, int64_t length) {
  assertx(length > 0);
  if (isClosed()) return 0;
  g_context->write(buffer, length);
  return length;
}