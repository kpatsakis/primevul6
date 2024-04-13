inline std::string format(CStringRef format_str, ArgList args) {
  MemoryWriter w;
  w.write(format_str, args);
  return w.str();
}