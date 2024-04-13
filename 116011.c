  void visit_bool(bool value) {
    if (spec_.type_) {
      visit_any_int(value);
      return;
    }
    write(value);
  }