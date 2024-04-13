  HTTP_CB(on_headers_complete) {
    Local<Value> cb = handle_->Get(on_headers_complete_sym);

    if (!cb->IsFunction())
      return 0;

    Local<Object> message_info = Object::New();

    if (have_flushed_) {
      // Slow case, flush remaining headers.
      Flush();
    }
    else {
      // Fast case, pass headers and URL to JS land.
      message_info->Set(headers_sym, CreateHeaders());
      if (parser_.type == HTTP_REQUEST)
        message_info->Set(url_sym, url_.ToString());
    }
    num_fields_ = num_values_ = 0;

    // METHOD
    if (parser_.type == HTTP_REQUEST) {
      message_info->Set(method_sym, method_to_str(parser_.method));
    }

    // STATUS
    if (parser_.type == HTTP_RESPONSE) {
      message_info->Set(status_code_sym, Integer::New(parser_.status_code));
    }

    // VERSION
    message_info->Set(version_major_sym, Integer::New(parser_.http_major));
    message_info->Set(version_minor_sym, Integer::New(parser_.http_minor));

    message_info->Set(should_keep_alive_sym,
        http_should_keep_alive(&parser_) ? True() : False());

    message_info->Set(upgrade_sym, parser_.upgrade ? True() : False());

    Local<Value> argv[1] = { message_info };

    Local<Value> head_response =
        Local<Function>::Cast(cb)->Call(handle_, 1, argv);

    if (head_response.IsEmpty()) {
      got_exception_ = true;
      return -1;
    }

    return head_response->IsTrue() ? 1 : 0;
  }