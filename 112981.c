void HTTP2Codec::onHeadersComplete(HTTPHeaderSize decodedSize,
                                   bool /*acknowledge*/) {
  decodeInfo_.onHeadersComplete(decodedSize);
  decodeInfo_.msg->setAdvancedProtocolString(http2::kProtocolString);

  HTTPMessage* msg = decodeInfo_.msg.get();
  HTTPRequestVerifier& verifier = decodeInfo_.verifier;
  if ((transportDirection_ == TransportDirection::DOWNSTREAM) &&
      verifier.hasUpgradeProtocol() &&
      (*msg->getUpgradeProtocol() == headers::kWebsocketString) &&
      msg->getMethod() == HTTPMethod::CONNECT) {
    msg->setIngressWebsocketUpgrade();
    ingressWebsocketUpgrade_ = true;
  } else {
    auto it = upgradedStreams_.find(curHeader_.stream);
    if (it != upgradedStreams_.end()) {
      upgradedStreams_.erase(curHeader_.stream);
      // a websocket upgrade was sent on this stream.
      if (msg->getStatusCode() != 200) {
        decodeInfo_.parsingError =
          folly::to<string>("Invalid response code to a websocket upgrade: ",
                            msg->getStatusCode());
        return;
      }
      msg->setIngressWebsocketUpgrade();
    }
  }
}