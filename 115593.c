  void visit(Sequence &ope) override {
    bool save_is_empty = false;
    const char *save_error_s = nullptr;
    std::string save_error_name;
    for (auto op : ope.opes_) {
      op->accept(*this);
      if (!is_empty) { return; }
      save_is_empty = is_empty;
      save_error_s = error_s;
      save_error_name = error_name;
      is_empty = false;
      error_name.clear();
    }
    is_empty = save_is_empty;
    error_s = save_error_s;
    error_name = save_error_name;
  }