  void Flush() {
    HandleScope scope;

    Local<Value> cb = handle_->Get(on_headers_sym);

    if (!cb->IsFunction())
      return;

    Handle<Value> argv[2] = {
      CreateHeaders(),
      url_.ToString()
    };

    Local<Value> r = Local<Function>::Cast(cb)->Call(handle_, 2, argv);

    if (r.IsEmpty())
      got_exception_ = true;

    url_.Reset();
    have_flushed_ = true;
  }