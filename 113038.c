ErrorCode HTTP2Codec::parsePriority(Cursor& cursor) {
  VLOG(4) << "parsing PRIORITY frame for stream=" << curHeader_.stream <<
    " length=" << curHeader_.length;
  http2::PriorityUpdate pri;
  auto err = http2::parsePriority(cursor, curHeader_, pri);
  RETURN_IF_ERROR(err);
  if (curHeader_.stream == pri.streamDependency) {
    streamError(folly::to<string>("Circular dependency for txn=",
                                  curHeader_.stream),
                ErrorCode::PROTOCOL_ERROR, false);
    return ErrorCode::NO_ERROR;
  }
  deliverCallbackIfAllowed(&HTTPCodec::Callback::onPriority, "onPriority",
                           curHeader_.stream,
                           std::make_tuple(pri.streamDependency,
                                           pri.exclusive,
                                           pri.weight));
  return ErrorCode::NO_ERROR;
}