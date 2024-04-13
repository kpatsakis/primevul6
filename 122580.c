  HTTP_CB(on_message_begin) {
    num_fields_ = num_values_ = 0;
    url_.Reset();
    return 0;
  }