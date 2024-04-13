  operator std::shared_ptr<Ope>() {
    return std::make_shared<WeakHolder>(holder_);
  }