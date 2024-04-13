void AcceptRoutingHandler<Pipeline, R>::read(
    Context*,
    AcceptPipelineType conn) {
  if (conn.type() != typeid(ConnInfo&)) {
    return;
  }

  populateAcceptors();

  const auto& connInfo = boost::get<ConnInfo&>(conn);
  auto socket = std::shared_ptr<folly::AsyncTransportWrapper>(
      connInfo.sock, folly::DelayedDestruction::Destructor());

  uint64_t connId = nextConnId_++;

  // Create a new routing pipeline for this connection to read from
  // the socket until it parses the routing data
  auto routingPipeline = newRoutingPipeline();
  routingPipeline->addBack(wangle::AsyncSocketHandler(socket));
  routingPipeline->addBack(routingHandlerFactory_->newHandler(connId, this));
  routingPipeline->finalize();

  // Initialize TransportInfo and set it on the routing pipeline
  auto transportInfo = std::make_shared<TransportInfo>(connInfo.tinfo);
  folly::SocketAddress localAddr, peerAddr;
  try {
    socket->getLocalAddress(&localAddr);
    socket->getPeerAddress(&peerAddr);
  } catch (...) {
    VLOG(2) << "Socket is no longer valid.";
    return;
  }
  transportInfo->localAddr = std::make_shared<folly::SocketAddress>(localAddr);
  transportInfo->remoteAddr = std::make_shared<folly::SocketAddress>(peerAddr);
  routingPipeline->setTransportInfo(transportInfo);

  routingPipeline->transportActive();
  routingPipelines_[connId] = std::move(routingPipeline);
}