  void Init(enum http_parser_type type) {
    http_parser_init(&parser_, type);
    url_.Reset();
    num_fields_ = 0;
    num_values_ = 0;
    have_flushed_ = false;
    got_exception_ = false;
  }