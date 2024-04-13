int64_t OutputFile::tell() {
  raise_warning("cannot tell a php://output stream");
  return -1;
}