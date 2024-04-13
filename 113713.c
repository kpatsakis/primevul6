bool BZ2File::close() {
  invokeFiltersOnClose();
  return closeImpl();
}