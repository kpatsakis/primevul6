bool BZ2File::closeImpl() {
  if (!isClosed()) {
    if (m_bzFile) {
      BZ2_bzclose(m_bzFile);
      m_bzFile = nullptr;
    }
    setIsClosed(true);
    if (m_innerFile) {
      m_innerFile->close();
    }
  }
  File::closeImpl();
  return true;
}