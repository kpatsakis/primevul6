  TProtocolPair getProtocol(transport::TTransportPair transports) override {
    return std::make_pair(
        inputFactory_->getProtocol(transports.first),
        outputFactory_->getProtocol(transports.second));
  }