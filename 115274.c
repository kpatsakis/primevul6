PrecedenceClimbing::get_reference_for_binop(Context &c) const {
  if (rule_.is_macro) {
    // Reference parameter in macro
    const auto &args = c.top_args();
    auto iarg = dynamic_cast<Reference &>(*binop_).iarg_;
    auto arg = args[iarg];
    return *dynamic_cast<Reference &>(*arg).rule_;
  }

  return *dynamic_cast<Reference &>(*binop_).rule_;
}