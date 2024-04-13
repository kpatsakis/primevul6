  Sequence(const Args &... args)
      : opes_{static_cast<std::shared_ptr<Ope>>(args)...} {}