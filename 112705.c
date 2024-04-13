void FastCGITransport::onHeadersComplete() {
  m_scriptName   = getParamTyped<std::string>("SCRIPT_FILENAME");
  m_docRoot      = getParamTyped<std::string>("DOCUMENT_ROOT");
  m_pathTrans    = getParamTyped<std::string>("PATH_TRANSLATED");
  m_serverObject = getParamTyped<std::string>("SCRIPT_NAME");

  if (!m_docRoot.empty() && *m_docRoot.rbegin() != '/') {
    m_docRoot += '/';
  }

  if (m_scriptName.empty() || RuntimeOption::ServerFixPathInfo) {
    // According to php-fpm, some servers don't set SCRIPT_FILENAME. In
    // this case, it uses PATH_TRANSLATED.
    // Added runtime option to change m_scriptFilename to s_pathTran
    // which will allow mod_fastcgi and mod_action to work correctly.
    m_scriptName = getPathTranslated();
  }

  [&] { // do a check for mod_proxy_fcgi and remove the extra portions of
        // the string
    if (!strncmp(m_scriptName.c_str(), "proxy:", sizeof("proxy:") - 1)) {
      folly::StringPiece newName {m_scriptName};

      // remove the proxy:type + :// from the start.
      auto proxyPos = newName.find("://");
      if (proxyPos == std::string::npos) return; // invalid mod_proxy

      newName.advance(proxyPos + sizeof("://"));

      // remove everything before the first / which is host:port
      auto slashPos = newName.find('/');
      if (slashPos == std::string::npos) {
        m_scriptName.clear(); // empty path
        return;
      }
      newName.advance(slashPos);

      // remove everything after the first ?
      auto questionPos = newName.find('?');
      if (questionPos != std::string::npos) {
        newName.subtract(newName.size() - questionPos);
      }

      m_scriptName = newName.str();
    }
  }();

  // RequestURI needs script_filename and path_translated to not include
  // the document root
  if (!m_scriptName.empty()) {
    if (m_scriptName.find(m_docRoot) == 0) {
      m_scriptName.erase(0, m_docRoot.length());
    } else {
      // if the document root isn't in the url set document root to /
      m_docRoot = "/";
    }
  }

  // XXX: This was originally done before remapping scriptName but that seemed
  // wrong as the value of docRoot may change. I haven't been able to confirm
  // that this is correct either.
  if (m_pathTrans.find(m_docRoot) == 0) {
    m_pathTrans.erase(0, m_docRoot.length());
  }

  auto qs = getParamTyped<std::string>("QUERY_STRING");
  if (!qs.empty()) {
    m_serverObject += "?";
    m_serverObject += qs;
  }

  // Treat everything apart from GET and HEAD as a post to be like php-src.
  auto const ex = getExtendedMethod();
  if (!strcmp(ex, "GET")) {
    m_method = Method::GET;
  } else if (!strcmp(ex, "HEAD")) {
    m_method = Method::HEAD;
  } else {
    m_method = Method::POST;
  }

  // IIS sets this value but sets it to off when SSL is off.
  if (m_requestParams.count("HTTPS") && !m_requestParams["HTTPS"].empty() &&
      strcasecmp(m_requestParams["HTTPS"].c_str(), "OFF")) {
    setSSL();
  }
}