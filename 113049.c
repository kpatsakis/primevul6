ErrorCode HTTP2Codec::parseHeadersImpl(
    Cursor& /*cursor*/,
    std::unique_ptr<IOBuf> headerBuf,
    const folly::Optional<http2::PriorityUpdate>& priority,
    const folly::Optional<uint32_t>& promisedStream,
    const folly::Optional<ExAttributes>& exAttributes) {
  curHeaderBlock_.append(std::move(headerBuf));
  std::unique_ptr<HTTPMessage> msg;
  if (curHeader_.flags & http2::END_HEADERS) {
    auto errorCode =
        parseHeadersDecodeFrames(priority, promisedStream, exAttributes, msg);
    if (errorCode.hasValue()) {
      return errorCode.value();
    }
  }

  // if we're not parsing CONTINUATION, then it's start of new header block
  if (curHeader_.type != http2::FrameType::CONTINUATION) {
    headerBlockFrameType_ = curHeader_.type;
  }

  // Report back what we've parsed
  if (callback_) {
    auto concurError = parseHeadersCheckConcurrentStreams(priority);
    if (concurError.hasValue()) {
      return concurError.value();
    }
    uint32_t headersCompleteStream = curHeader_.stream;
    bool trailers = parsingTrailers();
    bool allHeaderFramesReceived =
        (curHeader_.flags & http2::END_HEADERS) &&
        (headerBlockFrameType_ == http2::FrameType::HEADERS);
    if (allHeaderFramesReceived && !trailers) {
      // Only deliver onMessageBegin once per stream.
      // For responses with CONTINUATION, this will be delayed until
      // the frame with the END_HEADERS flag set.
      if (!deliverCallbackIfAllowed(&HTTPCodec::Callback::onMessageBegin,
                                    "onMessageBegin",
                                    curHeader_.stream,
                                    msg.get())) {
        return handleEndStream();
      }
   } else if (curHeader_.type == http2::FrameType::EX_HEADERS) {
      if (!deliverCallbackIfAllowed(&HTTPCodec::Callback::onExMessageBegin,
                                    "onExMessageBegin",
                                    curHeader_.stream,
                                    exAttributes->controlStream,
                                    exAttributes->unidirectional,
                                    msg.get())) {
        return handleEndStream();
      }
    } else if (curHeader_.type == http2::FrameType::PUSH_PROMISE) {
      DCHECK(promisedStream);
      if (!deliverCallbackIfAllowed(&HTTPCodec::Callback::onPushMessageBegin,
                                    "onPushMessageBegin", *promisedStream,
                                    curHeader_.stream, msg.get())) {
        return handleEndStream();
      }
      headersCompleteStream = *promisedStream;
    }

    if (curHeader_.flags & http2::END_HEADERS && msg) {
      if (!(curHeader_.flags & http2::END_STREAM)) {
        // If it there are DATA frames coming, consider it chunked
        msg->setIsChunked(true);
      }
      if (trailers) {
        VLOG(4) << "Trailers complete for streamId=" << headersCompleteStream
                << " direction=" << transportDirection_;
        auto trailerHeaders =
            std::make_unique<HTTPHeaders>(msg->extractHeaders());
        msg.reset();
        callback_->onTrailersComplete(headersCompleteStream,
                                      std::move(trailerHeaders));
      } else {
        callback_->onHeadersComplete(headersCompleteStream, std::move(msg));
      }
    }
    return handleEndStream();
  }
  return ErrorCode::NO_ERROR;
}