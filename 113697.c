bool Capability::ChangeUnixUser(const std::string &username) {
  if (!username.empty()) {
    struct passwd *pw = getpwnam(username.c_str());
    if (pw && pw->pw_uid) {
      return ChangeUnixUser(pw->pw_uid);
    }
  }
  return false;
}