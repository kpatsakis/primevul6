bool IsUrlArg(const base::CommandLine::CharType* arg) {
  // the first character must be a letter for this to be a URL
  auto c = *arg;
  if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z')) {
    for (auto p = arg + 1; *p; ++p) {
      c = *p;

      // colon indicates that the argument starts with a URI scheme
      if (c == ':') {
        // it could also be a Windows filesystem path
        if (p == arg + 1)
          break;

        return true;
      }

      // white-space before a colon means it's not a URL
      if (c == ' ' || (0x9 <= c && c <= 0xD))
        break;
    }
  }

  return false;
}