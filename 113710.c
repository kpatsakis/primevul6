bool OutputFile::flush() {
  if (!isClosed()) {
    g_context->flush();
    return true;
  }
  return false;
}