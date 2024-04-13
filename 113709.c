bool OutputFile::closeImpl() {
  *s_pcloseRet = 0;
  if (!isClosed()) {
    setIsClosed(true);
    return true;
  }
  return false;
}