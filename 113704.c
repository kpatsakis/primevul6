int64_t BZ2File::errnu() {
  assertx(m_bzFile);
  int errnum = 0;
  BZ2_bzerror(m_bzFile, &errnum);
  return errnum;
}