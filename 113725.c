bool OutputFile::open(const String& /*filename*/, const String& /*mode*/) {
  raise_fatal_error("cannot open a php://output file ");
}