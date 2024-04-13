inline const char *Holder::trace_name() const {
  if (trace_name_.empty()) { trace_name_ = "[" + outer_->name + "]"; }
  return trace_name_.c_str();
}