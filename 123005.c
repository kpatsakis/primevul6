  void whenInputIs(const char *json) {
    strcpy(_jsonString, json);
    _result = QuotedString::extractFrom(_jsonString, &_trailing);
  }