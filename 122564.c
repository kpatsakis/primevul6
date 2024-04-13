void InitHttpParser(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(Parser::New);
  t->InstanceTemplate()->SetInternalFieldCount(1);
  t->SetClassName(String::NewSymbol("HTTPParser"));

  PropertyAttribute attrib = (PropertyAttribute) (ReadOnly | DontDelete);
  t->Set(String::NewSymbol("REQUEST"), Integer::New(HTTP_REQUEST), attrib);
  t->Set(String::NewSymbol("RESPONSE"), Integer::New(HTTP_RESPONSE), attrib);

  NODE_SET_PROTOTYPE_METHOD(t, "execute", Parser::Execute);
  NODE_SET_PROTOTYPE_METHOD(t, "finish", Parser::Finish);
  NODE_SET_PROTOTYPE_METHOD(t, "reinitialize", Parser::Reinitialize);

  target->Set(String::NewSymbol("HTTPParser"), t->GetFunction());

  on_headers_sym          = NODE_PSYMBOL("onHeaders");
  on_headers_complete_sym = NODE_PSYMBOL("onHeadersComplete");
  on_body_sym             = NODE_PSYMBOL("onBody");
  on_message_complete_sym = NODE_PSYMBOL("onMessageComplete");

  delete_sym = NODE_PSYMBOL("DELETE");
  get_sym = NODE_PSYMBOL("GET");
  head_sym = NODE_PSYMBOL("HEAD");
  post_sym = NODE_PSYMBOL("POST");
  put_sym = NODE_PSYMBOL("PUT");
  connect_sym = NODE_PSYMBOL("CONNECT");
  options_sym = NODE_PSYMBOL("OPTIONS");
  trace_sym = NODE_PSYMBOL("TRACE");
  patch_sym = NODE_PSYMBOL("PATCH");
  copy_sym = NODE_PSYMBOL("COPY");
  lock_sym = NODE_PSYMBOL("LOCK");
  mkcol_sym = NODE_PSYMBOL("MKCOL");
  move_sym = NODE_PSYMBOL("MOVE");
  propfind_sym = NODE_PSYMBOL("PROPFIND");
  proppatch_sym = NODE_PSYMBOL("PROPPATCH");
  unlock_sym = NODE_PSYMBOL("UNLOCK");
  report_sym = NODE_PSYMBOL("REPORT");
  mkactivity_sym = NODE_PSYMBOL("MKACTIVITY");
  checkout_sym = NODE_PSYMBOL("CHECKOUT");
  merge_sym = NODE_PSYMBOL("MERGE");
  msearch_sym = NODE_PSYMBOL("M-SEARCH");
  notify_sym = NODE_PSYMBOL("NOTIFY");
  subscribe_sym = NODE_PSYMBOL("SUBSCRIBE");
  unsubscribe_sym = NODE_PSYMBOL("UNSUBSCRIBE");;
  unknown_method_sym = NODE_PSYMBOL("UNKNOWN_METHOD");

  method_sym = NODE_PSYMBOL("method");
  status_code_sym = NODE_PSYMBOL("statusCode");
  http_version_sym = NODE_PSYMBOL("httpVersion");
  version_major_sym = NODE_PSYMBOL("versionMajor");
  version_minor_sym = NODE_PSYMBOL("versionMinor");
  should_keep_alive_sym = NODE_PSYMBOL("shouldKeepAlive");
  upgrade_sym = NODE_PSYMBOL("upgrade");
  headers_sym = NODE_PSYMBOL("headers");
  url_sym = NODE_PSYMBOL("url");

  settings.on_message_begin    = Parser::on_message_begin;
  settings.on_url              = Parser::on_url;
  settings.on_header_field     = Parser::on_header_field;
  settings.on_header_value     = Parser::on_header_value;
  settings.on_headers_complete = Parser::on_headers_complete;
  settings.on_body             = Parser::on_body;
  settings.on_message_complete = Parser::on_message_complete;
}