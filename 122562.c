  void Save() {
    url_.Save();

    for (int i = 0; i < num_fields_; i++) {
      fields_[i].Save();
    }

    for (int i = 0; i < num_values_; i++) {
      values_[i].Save();
    }
  }