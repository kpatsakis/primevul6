void FastCGITransport::sendResponseHeaders(IOBufQueue& queue, int code) {
  auto appender = [&](folly::StringPiece sp) mutable { queue.append(sp); };
  if (code != 200) {
    auto info = getResponseInfo();
    auto reason = !info.empty() ? info : HttpProtocol::GetReasonString(code);

    folly::format("Status: {} {}\r\n", code, reason)(appender);
  }

  for (auto& header : m_responseHeaders) {
    for (auto& value : header.second) {
      folly::format("{}: {}\r\n", header.first, value)(appender);
    }
  }

  queue.append("\r\n", 2);
}