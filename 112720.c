String StringUtil::HtmlEncode(const String& input, const int64_t qsBitmask,
                              const char *charset, bool dEncode, bool htmlEnt) {
  if (input.empty()) return input;

  assert(charset);
  bool utf8 = true;
  if (strcasecmp(charset, "ISO-8859-1") == 0) {
    utf8 = false;
  } else if (strcasecmp(charset, "UTF-8")) {
    throw_not_implemented(charset);
  }

  int len = input.size();
  char *ret = string_html_encode(input.data(), len,
                                 qsBitmask, utf8, dEncode, htmlEnt);
  if (!ret) {
    return empty_string();
  }
  return String(ret, len, AttachString);
}