  TProtocolPair getProtocol(transport::TTransportPair transports) override {
    return std::make_pair(
        factory_->getProtocol(transports.first),
        factory_->getProtocol(transports.second));
  }