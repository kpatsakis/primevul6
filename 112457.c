  Variant getErrorCode() {
    if (!m_ucoll) {
      Logger::Verbose("m_ucoll is NULL");
      return false;
    }
    return m_errcode.getErrorCode();
  }