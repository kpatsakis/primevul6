bool BZ2File::eof() {
  assertx(m_bzFile);
  return getEof();
}