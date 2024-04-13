  void addPriorityNode(HTTPCodec::StreamID id, HTTPCodec::StreamID) override {
    nodes_.push_back(id);
  }