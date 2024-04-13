std::string FastCGITransport::getHeader(const char* name) {
  std::string key("HTTP_");
  for (auto p = name; *p; ++p) {
    key += (*p == '-') ? '_' : toupper(*p);
  }

  if (m_requestParams.count(key)) {
    return m_requestParams[key];
  }

  // Special headers that are not prefixed with HTTP_
  if (strcasecmp(name, "Authorization") == 0) {
    return getParamTyped<std::string>("Authorization");
  }

  if (strcasecmp(name, "Content-Length") == 0) {
    return getParamTyped<std::string>("CONTENT_LENGTH");
  }

  if (strcasecmp(name, "Content-Type") == 0) {
    return getParamTyped<std::string>("CONTENT_TYPE");
  }

  return "";
}