void std_format(const T &value, std::basic_string<Char> &result) {
  std::basic_ostringstream<Char> os;
  os << value;
  result = os.str();
}