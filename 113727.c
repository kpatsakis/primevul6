int OutputFile::getc() {
  raise_warning("cannot read from a php://output stream");
  return -1;
}