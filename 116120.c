  void visit_wstring(internal::Arg::StringValue<Char> value) {
    writer_.write_str(value, spec_);
  }