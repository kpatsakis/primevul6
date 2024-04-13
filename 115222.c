TEST(FixedLengthFrameDecoder, FailWhenLengthFieldEndOffset) {
  auto pipeline = Pipeline<IOBufQueue&, std::unique_ptr<IOBuf>>::create();
  int called = 0;

  (*pipeline)
    .addBack(FixedLengthFrameDecoder(10))
    .addBack(test::FrameTester([&](std::unique_ptr<IOBuf> buf) {
        auto sz = buf->computeChainDataLength();
        called++;
        EXPECT_EQ(sz, 10);
      }))
    .finalize();

  auto buf3 = createZeroedBuffer(3);
  auto buf11 = createZeroedBuffer(11);
  auto buf16 = createZeroedBuffer(16);

  IOBufQueue q(IOBufQueue::cacheChainLength());

  q.append(std::move(buf3));
  pipeline->read(q);
  EXPECT_EQ(called, 0);

  q.append(std::move(buf11));
  pipeline->read(q);
  EXPECT_EQ(called, 1);

  q.append(std::move(buf16));
  pipeline->read(q);
  EXPECT_EQ(called, 3);
}