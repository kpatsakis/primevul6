  bool setLocale(const String& locale) {
    if (m_locale.same(locale)) {
      return true;
    }
    if (m_ucoll) {
      ucol_close(m_ucoll);
      m_ucoll = NULL;
    }
    m_errcode.clearError();
    UErrorCode error = U_ZERO_ERROR;
    m_ucoll = ucol_open(locale.data(), &error);
    if (m_ucoll == NULL) {
      raise_warning("failed to load %s locale from icu data", locale.data());
      return false;
    }
    if (U_FAILURE(error)) {
      m_errcode.setError(error);
      ucol_close(m_ucoll);
      m_ucoll = NULL;
      return false;
    }
    m_locale = locale;
    return true;
  }