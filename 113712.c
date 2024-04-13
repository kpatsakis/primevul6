bool OutputFile::rewind() {
  raise_warning("cannot rewind a php://output stream");
  return false;
}