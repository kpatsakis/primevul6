bool is_uploaded_file(const std::string filename) {
  std::set<std::string> &rfc1867UploadedFiles =
    s_rfc1867_data->rfc1867UploadedFiles;
  return rfc1867UploadedFiles.find(filename) != rfc1867UploadedFiles.end();
}