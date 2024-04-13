String BZ2File::errstr() {
  assertx(m_bzFile);
  int errnum;
  return BZ2_bzerror(m_bzFile, &errnum);
}