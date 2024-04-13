  void requestShutdown() override {
    m_locale.reset();
    m_errcode.clearError(false);
    if (m_ucoll) {
      ucol_close(m_ucoll);
      m_ucoll = NULL;
    }
  }