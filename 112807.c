  void requestInit() override {
    if (RuntimeOption::EnableUploadProgress) {
      rfc1867Callback = apc_rfc1867_progress;
    } else {
      rfc1867Callback = nullptr;
    }
  }