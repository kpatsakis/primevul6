  void requestShutdown() override {
    if (!rfc1867UploadedFiles.empty()) destroy_uploaded_files();
  }