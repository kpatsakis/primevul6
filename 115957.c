void std_format(long double value, std::wstring &result) {
  wchar_t buffer[100];
  swprintf(buffer, L"%Lg", value);
  result = buffer;
}