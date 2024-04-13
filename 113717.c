bool BZ2File::open(const String& filename, const String& mode) {
  assertx(m_bzFile == nullptr);

  return m_innerFile->open(filename, mode) &&
    (m_bzFile = BZ2_bzdopen(dup(m_innerFile->fd()), mode.data()));
}