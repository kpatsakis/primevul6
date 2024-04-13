OutputFile::OutputFile(const String& filename): File(true, s_php, s_output) {
  if (filename != s_php_output) {
    raise_fatal_error("not a php://output file ");
  }
  setIsLocal(true);
}