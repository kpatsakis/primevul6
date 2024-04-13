  HTTP_DATA_CB(on_body) {
    HandleScope scope;

    Local<Value> cb = handle_->Get(on_body_sym);
    if (!cb->IsFunction())
      return 0;

    Handle<Value> argv[3] = {
      *current_buffer,
      Integer::New(at - current_buffer_data),
      Integer::New(length)
    };

    Local<Value> r = Local<Function>::Cast(cb)->Call(handle_, 3, argv);

    if (r.IsEmpty()) {
      got_exception_ = true;
      return -1;
    }

    return 0;
  }