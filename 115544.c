  std::vector<std::string> get_rule_names() {
    std::vector<std::string> rules;
    rules.reserve(grammar_->size());
    for (auto const &r : *grammar_) {
      rules.emplace_back(r.first);
    }
    return rules;
  }