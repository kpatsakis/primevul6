  void set_string(StringRef str) {
    string.value = str.data();
    string.size = str.size();
  }