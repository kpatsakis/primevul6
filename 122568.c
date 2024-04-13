  static Handle<Value> New(const Arguments& args) {
    HandleScope scope;

    http_parser_type type =
        static_cast<http_parser_type>(args[0]->Int32Value());

    if (type != HTTP_REQUEST && type != HTTP_RESPONSE) {
      return ThrowException(Exception::Error(String::New(
          "Argument must be HTTPParser.REQUEST or HTTPParser.RESPONSE")));
    }

    Parser* parser = new Parser(type);
    parser->Wrap(args.This());

    return args.This();
  }