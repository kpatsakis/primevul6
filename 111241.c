  bool Get(const string& key, string* value) override {
    std::unique_ptr<table::Iterator> iter(table_->NewIterator());
    iter->Seek(key);
    if (iter->Valid() && iter->key() == key) {
      StringPiece v = iter->value();
      value->assign(v.data(), v.size());
      return true;
    } else {
      return false;
    }
  }