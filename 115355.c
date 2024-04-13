inline size_t Holder::parse_core(const char *s, size_t n, SemanticValues &sv,
                                 Context &c, any &dt) const {
  if (!ope_) {
    throw std::logic_error("Uninitialized definition ope was used...");
  }

  // Macro reference
  if (outer_->is_macro) {
    c.rule_stack.push_back(outer_);
    auto len = ope_->parse(s, n, sv, c, dt);
    c.rule_stack.pop_back();
    return len;
  }

  size_t len;
  any val;

  c.packrat(s, outer_->id, len, val, [&](any &a_val) {
    if (outer_->enter) { outer_->enter(s, n, dt); }

    auto se2 = make_scope_exit([&]() {
      c.pop();

      if (outer_->leave) { outer_->leave(s, n, len, a_val, dt); }
    });

    auto &chldsv = c.push();

    c.rule_stack.push_back(outer_);
    len = ope_->parse(s, n, chldsv, c, dt);
    c.rule_stack.pop_back();

    // Invoke action
    if (success(len)) {
      chldsv.s_ = s;
      chldsv.n_ = len;
      chldsv.name_ = outer_->name;

      if (!IsPrioritizedChoice::check(*ope_)) {
        chldsv.choice_count_ = 0;
        chldsv.choice_ = 0;
      }

      try {
        a_val = reduce(chldsv, dt);
      } catch (const parse_error &e) {
        if (e.what()) {
          if (c.message_pos < s) {
            c.message_pos = s;
            c.message = e.what();
          }
        }
        len = static_cast<size_t>(-1);
      }
    }
  });

  if (success(len)) {
    if (!outer_->ignoreSemanticValue) {
      sv.emplace_back(std::move(val));
      sv.tags.emplace_back(str2tag(outer_->name.c_str()));
    }
  } else {
    if (outer_->error_message) {
      if (c.message_pos < s) {
        c.message_pos = s;
        c.message = outer_->error_message();
      }
    }
  }

  return len;
}