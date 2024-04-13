TEST(LineBasedFrameDecoder, NewLineOnly) {
  auto pipeline = Pipeline<IOBufQueue&, std::unique_ptr<IOBuf>>::create();
  int called = 0;

  (*pipeline)
    .addBack(LineBasedFrameDecoder(
               10, true, LineBasedFrameDecoder::TerminatorType::NEWLINE))
    .addBack(test::FrameTester([&](std::unique_ptr<IOBuf> buf) {
        auto sz = buf->computeChainDataLength();
        called++;
        EXPECT_EQ(sz, 1);
      }))
    .finalize();

  auto buf = createZeroedBuffer(2);
  RWPrivateCursor c(buf.get());
  c.write<char>('\r');
  c.write<char>('\n');

  IOBufQueue q(IOBufQueue::cacheChainLength());

  q.append(std::move(buf));
  pipeline->read(q);
  EXPECT_EQ(called, 1);
}