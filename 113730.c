bool BZ2File::flush() {
  assertx(m_bzFile);
  return BZ2_bzflush(m_bzFile);
}