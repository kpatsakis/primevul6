inline std::wstring format(WCStringRef format_str, ArgList args) {
  WMemoryWriter w;
  w.write(format_str, args);
  return w.str();
}