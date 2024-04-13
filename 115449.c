  Definition &operator<=(const std::shared_ptr<Ope> &ope) {
    holder_->ope_ = ope;
    return *this;
  }