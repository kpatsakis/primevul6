  void set_error() {
    is_empty = true;
    error_s = refs_.back().first;
    error_name = refs_.back().second;
  }