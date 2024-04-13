Array BZ2File::error() {
  assertx(m_bzFile);
  int errnum;
  const char * errstr;
  errstr = BZ2_bzerror(m_bzFile, &errnum);
  return make_map_array(s_errno, errnum, s_errstr, String(errstr));
}