bool OutputFile::close() {
  invokeFiltersOnClose();
  return closeImpl();
}