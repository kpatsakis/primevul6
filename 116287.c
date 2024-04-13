std::string format_message(int id, const char *format,
    const fmt::ArgList &args) {
  MemoryWriter w;
  w.write("[{}] ", id);
  w.write(format, args);
  return w.str();
}