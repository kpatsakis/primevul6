  static Handle<Value> Execute(const Arguments& args) {
    HandleScope scope;

    Parser* parser = ObjectWrap::Unwrap<Parser>(args.This());

    assert(!current_buffer);
    assert(!current_buffer_data);

    if (current_buffer) {
      return ThrowException(Exception::TypeError(
            String::New("Already parsing a buffer")));
    }

    Local<Value> buffer_v = args[0];

    if (!Buffer::HasInstance(buffer_v)) {
      return ThrowException(Exception::TypeError(
            String::New("Argument should be a buffer")));
    }

    Local<Object> buffer_obj = buffer_v->ToObject();
    char *buffer_data = Buffer::Data(buffer_obj);
    size_t buffer_len = Buffer::Length(buffer_obj);

    size_t off = args[1]->Int32Value();
    if (off >= buffer_len) {
      return ThrowException(Exception::Error(
            String::New("Offset is out of bounds")));
    }

    size_t len = args[2]->Int32Value();
    if (off+len > buffer_len) {
      return ThrowException(Exception::Error(
            String::New("Length is extends beyond buffer")));
    }

    // Assign 'buffer_' while we parse. The callbacks will access that varible.
    current_buffer = &buffer_v;
    current_buffer_data = buffer_data;
    current_buffer_len = buffer_len;
    parser->got_exception_ = false;

    size_t nparsed =
      http_parser_execute(&parser->parser_, &settings, buffer_data + off, len);

    parser->Save();

    // Unassign the 'buffer_' variable
    assert(current_buffer);
    current_buffer = NULL;
    current_buffer_data = NULL;

    // If there was an exception in one of the callbacks
    if (parser->got_exception_) return Local<Value>();

    Local<Integer> nparsed_obj = Integer::New(nparsed);
    // If there was a parse error in one of the callbacks
    // TODO What if there is an error on EOF?
    if (!parser->parser_.upgrade && nparsed != len) {
      Local<Value> e = Exception::Error(String::NewSymbol("Parse Error"));
      Local<Object> obj = e->ToObject();
      obj->Set(String::NewSymbol("bytesParsed"), nparsed_obj);
      return scope.Close(e);
    } else {
      return scope.Close(nparsed_obj);
    }
  }