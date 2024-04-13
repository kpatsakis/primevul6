  void set_string(WStringRef str) {
    wstring.value = str.data();
    wstring.size = str.size();
  }