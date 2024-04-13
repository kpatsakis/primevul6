  explicit LiteSessionWrapper(std::unique_ptr<Session> wrapped)
      : wrapped_(std::move(wrapped)) {}