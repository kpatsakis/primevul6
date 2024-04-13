void AcceptRoutingHandler<Pipeline, R>::onError(
    uint64_t connId,
    folly::exception_wrapper ex) {
  VLOG(4) << "Exception while parsing routing data: " << ex.what();

  // Notify all handlers of the exception
  auto ctx = getContext();
  auto pipeline =
      CHECK_NOTNULL(dynamic_cast<AcceptPipeline*>(ctx->getPipeline()));
  pipeline->readException(ex);

  // Delete the routing pipeline. This will close and delete the socket as well.
  routingPipelines_.erase(connId);
}