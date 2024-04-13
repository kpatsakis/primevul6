inline size_t PrecedenceClimbing::parse_expression(const char *s, size_t n,
                                                   SemanticValues &sv,
                                                   Context &c, any &dt,
                                                   size_t min_prec) const {
  auto len = atom_->parse(s, n, sv, c, dt);
  if (fail(len)) { return len; }

  std::string tok;
  auto &rule = get_reference_for_binop(c);
  auto action = rule.action;

  rule.action = [&](SemanticValues &sv2, any &dt2) -> any {
    tok = sv2.token();
    if (action) {
      return action(sv2, dt2);
    } else if (!sv2.empty()) {
      return sv2[0];
    }
    return any();
  };
  auto action_se = make_scope_exit([&]() { rule.action = action; });

  auto save_error_pos = c.error_pos;

  auto i = len;
  while (i < n) {
    std::vector<any> save_values(sv.begin(), sv.end());
    auto save_tokens = sv.tokens;

    auto chv = c.push();
    auto chl = binop_->parse(s + i, n - i, chv, c, dt);
    c.pop();

    if (fail(chl)) {
      c.error_pos = save_error_pos;
      break;
    }

    auto it = info_.find(tok);
    if (it == info_.end()) { break; }

    auto level = std::get<0>(it->second);
    auto assoc = std::get<1>(it->second);

    if (level < min_prec) { break; }

    sv.emplace_back(std::move(chv[0]));
    i += chl;

    auto next_min_prec = level;
    if (assoc == 'L') { next_min_prec = level + 1; }

    chv = c.push();
    chl = parse_expression(s + i, n - i, chv, c, dt, next_min_prec);
    c.pop();

    if (fail(chl)) {
      sv.assign(save_values.begin(), save_values.end());
      sv.tokens = save_tokens;
      c.error_pos = save_error_pos;
      break;
    }

    sv.emplace_back(std::move(chv[0]));
    i += chl;

    any val;
    if (rule_.action) {
      sv.s_ = s;
      sv.n_ = i;
      val = rule_.action(sv, dt);
    } else if (!sv.empty()) {
      val = sv[0];
    }
    sv.clear();
    sv.emplace_back(std::move(val));
  }

  return i;
}