size_t HTTP2Codec::addPriorityNodes(
    PriorityQueue& queue,
    folly::IOBufQueue& writeBuf,
    uint8_t maxLevel) {
  HTTPCodec::StreamID parent = 0;
  size_t bytes = 0;
  while (maxLevel--) {
    auto id = createStream();
    virtualPriorityNodes_.push_back(id);
    queue.addPriorityNode(id, parent);
    bytes += generatePriority(writeBuf, id, std::make_tuple(parent, false, 0));
    parent = id;
  }
  return bytes;
}