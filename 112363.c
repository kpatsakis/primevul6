  bool setAttribute(int64_t attr, int64_t val) {
    if (!m_ucoll) {
      Logger::Verbose("m_ucoll is NULL");
      return false;
    }
    m_errcode.clearError();
    UErrorCode error = U_ZERO_ERROR;
    ucol_setAttribute(m_ucoll, (UColAttribute)attr,
                      (UColAttributeValue)val, &error);
    if (U_FAILURE(error)) {
      m_errcode.setError(error);
      Logger::Verbose("Error setting attribute value");
      return false;
    }
    return true;
  }