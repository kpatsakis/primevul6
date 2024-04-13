void LineBasedFrameDecoder::fail(Context* ctx, std::string len) {
  ctx->fireReadException(
    folly::make_exception_wrapper<std::runtime_error>(
      "frame length" + len +
      " exeeds max " + folly::to<std::string>(maxLength_)));
}