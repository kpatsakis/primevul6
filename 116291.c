  void visit_string(internal::Arg::StringValue<char> value) {
    writer_.write_str(value, spec_);
  }