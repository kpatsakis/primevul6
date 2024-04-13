  void requestInit() override {
    m_locale = String(uloc_getDefault(), CopyString);
    m_errcode.clearError();
    UErrorCode error = U_ZERO_ERROR;
    m_ucoll = ucol_open(m_locale.data(), &error);
    if (U_FAILURE(error)) {
      m_errcode.setError(error);
    }
    assert(m_ucoll);
  }