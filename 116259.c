void std_format(long double value, std::string &result) {
  char buffer[100];
  safe_sprintf(buffer, "%Lg", value);
  result = buffer;
}