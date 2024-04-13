TEST(LineBasedFrameDecoder, DoubleCarriage) {
  auto pipeline = Pipeline<IOBufQueue&, std::unique_ptr<IOBuf>>::create();
  int called = 0;

  (*pipeline)
      .addBack(LineBasedFrameDecoder(
          10, true, LineBasedFrameDecoder::TerminatorType::CARRIAGENEWLINE))
      .addBack(test::FrameTester([&](std::unique_ptr<IOBuf> buf) {
        auto sz = buf->computeChainDataLength();
        called++;
        EXPECT_EQ(sz, 1);
      }))
      .finalize();

  IOBufQueue q(IOBufQueue::cacheChainLength());
  q.append(IOBuf::copyBuffer("\r\r\na\r\n"));
  pipeline->read(q);
  EXPECT_EQ(called, 2);
}