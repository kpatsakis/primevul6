  explicit TProtocol(TTransport* ptrans)
      : ptrans_(ptrans, [](TTransport*) {}) {}