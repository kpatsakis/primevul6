  static Handle<Value> Finish(const Arguments& args) {
    HandleScope scope;

    Parser* parser = ObjectWrap::Unwrap<Parser>(args.This());

    assert(!current_buffer);
    parser->got_exception_ = false;

    int rv = http_parser_execute(&(parser->parser_), &settings, NULL, 0);

    if (parser->got_exception_) return Local<Value>();

    if (rv != 0) {
      Local<Value> e = Exception::Error(String::NewSymbol("Parse Error"));
      Local<Object> obj = e->ToObject();
      obj->Set(String::NewSymbol("bytesParsed"), Integer::New(0));
      return scope.Close(e);
    }

    return Undefined();
  }