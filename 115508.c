  std::string token(size_t id = 0) const {
    if (!tokens.empty()) {
      assert(id < tokens.size());
      const auto &tok = tokens[id];
      return std::string(tok.first, tok.second);
    }
    return std::string(s_, n_);
  }