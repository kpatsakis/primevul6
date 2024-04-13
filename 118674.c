  TDualProtocolFactory(
      std::shared_ptr<TProtocolFactory> inputFactory,
      std::shared_ptr<TProtocolFactory> outputFactory)
      : inputFactory_(inputFactory), outputFactory_(outputFactory) {}