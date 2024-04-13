std::string base64url_decode(const std::string& str) {
  return proxygen::Base64::urlDecode(str);
}