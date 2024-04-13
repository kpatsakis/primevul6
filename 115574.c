  Definition(const Definition &rhs) : name(rhs.name), holder_(rhs.holder_) {
    holder_->outer_ = this;
  }