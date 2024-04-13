  Definition(const std::shared_ptr<Ope> &ope)
      : holder_(std::make_shared<Holder>(this)) {
    *this <= ope;
  }