BZ2File::BZ2File(req::ptr<PlainFile>&& innerFile)
: m_bzFile(nullptr), m_innerFile(std::move(innerFile)) {
  setIsLocal(m_innerFile->isLocal());
}