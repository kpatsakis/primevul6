  HTTP_CB(on_message_complete) {
    HandleScope scope;

    if (num_fields_)
      Flush(); // Flush trailing HTTP headers.

    Local<Value> cb = handle_->Get(on_message_complete_sym);

    if (!cb->IsFunction())
      return 0;

    Local<Value> r = Local<Function>::Cast(cb)->Call(handle_, 0, NULL);

    if (r.IsEmpty()) {
      got_exception_ = true;
      return -1;
    }

    return 0;
  }