std::string FastCGITransport::unmangleHeader(const std::string& name) const {
  if (name == "Authorization") {
    return name; // Already unmangled
  }

  if (name == "CONTENT_LENGTH") {
    return "Content-Length";
  }

  if (name == "CONTENT_TYPE") {
    return "Content-Type";
  }

  if (strncasecmp(name.c_str(), "HTTP_", 5)) {
    return "";
  }

  std::string ret;
  bool is_upper = true;
  for (auto const& c : folly::StringPiece(name, 5)) {
    if (c == '_') {
      ret += '-';
      is_upper = true;
    } else {
      ret += is_upper ? toupper(c) : tolower(c);
      is_upper = false;
    }
  }
  return ret;
}