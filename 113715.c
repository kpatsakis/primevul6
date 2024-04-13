int64_t BZ2File::writeImpl(const char * buf, int64_t length) {
  assertx(m_bzFile);
  return BZ2_bzwrite(m_bzFile, (char *)buf, length);
}