bool LineBasedFrameDecoder::decode(Context* ctx,
                                   IOBufQueue& buf,
                                   std::unique_ptr<IOBuf>& result,
                                   size_t&) {
  int64_t eol = findEndOfLine(buf);

  if (!discarding_) {
    if (eol >= 0) {
      Cursor c(buf.front());
      c += eol;
      auto delimLength = c.read<char>() == '\r' ? 2 : 1;
      if (eol > maxLength_) {
        buf.split(eol + delimLength);
        fail(ctx, folly::to<std::string>(eol));
        return false;
      }

      std::unique_ptr<folly::IOBuf> frame;

      if (stripDelimiter_) {
        frame = buf.split(eol);
        buf.trimStart(delimLength);
      } else {
        frame = buf.split(eol + delimLength);
      }

      result = std::move(frame);
      return true;
    } else {
      auto len = buf.chainLength();
      if (len > maxLength_) {
        discardedBytes_ = len;
        buf.trimStart(len);
        discarding_ = true;
        fail(ctx, "over " + folly::to<std::string>(len));
      }
      return false;
    }
  } else {
    if (eol >= 0) {
      Cursor c(buf.front());
      c += eol;
      auto delimLength = c.read<char>() == '\r' ? 2 : 1;
      buf.trimStart(eol + delimLength);
      discardedBytes_ = 0;
      discarding_ = false;
    } else {
      discardedBytes_ = buf.chainLength();
      buf.move();
    }

    return false;
  }
}