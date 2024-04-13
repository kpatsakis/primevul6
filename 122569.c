  Parser(enum http_parser_type type) : ObjectWrap() {
    Init(type);
  }