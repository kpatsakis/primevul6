static void destroy_uploaded_files() {
  std::set<std::string> &rfc1867UploadedFiles =
    s_rfc1867_data->rfc1867UploadedFiles;
  for (auto iter = rfc1867UploadedFiles.begin();
       iter != rfc1867UploadedFiles.end(); iter++) {
    unlink(iter->c_str());
  }
  rfc1867UploadedFiles.clear();
}