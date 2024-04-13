bool OutputFile::seek(int64_t /*offset*/, int /*whence*/ /* = SEEK_SET */) {
  raise_warning("cannot seek a php://output stream");
  return false;
}