inline constexpr unsigned int str2tag(const char *str, unsigned int h = 0) {
  return (*str == '\0')
             ? h
             : str2tag(str + 1, (h * 33) ^ static_cast<unsigned char>(*str));
}