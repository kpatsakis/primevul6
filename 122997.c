  void whenInputIs(const char *json, size_t len) {
    memcpy(_jsonString, json, len);
    _result = QuotedString::extractFrom(_jsonString, &_trailing);
  }