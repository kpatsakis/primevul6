std::string base64url_encode(ByteRange range) {
  return proxygen::Base64::urlEncode(range);
}