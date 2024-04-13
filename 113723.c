BZ2File::BZ2File(): m_bzFile(nullptr), m_innerFile(req::make<PlainFile>()) {
  m_innerFile->unregister();
  setIsLocal(m_innerFile->isLocal());
}