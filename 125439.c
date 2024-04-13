void AcceptRoutingHandler<Pipeline, R>::populateAcceptors() {
  if (!acceptors_.empty()) {
    return;
  }
  CHECK(server_);
  server_->forEachWorker(
      [&](Acceptor* acceptor) { acceptors_.push_back(acceptor); });
}