TEST(LengthFieldFrameDecoderStrip, PrePostHeader) {
  auto pipeline = Pipeline<IOBufQueue&, std::unique_ptr<IOBuf>>::create();
  int called = 0;

  (*pipeline)
    .addBack(LengthFieldBasedFrameDecoder(2, 10, 2, 2, 4))
    .addBack(test::FrameTester([&](std::unique_ptr<IOBuf> buf) {
        auto sz = buf->computeChainDataLength();
        called++;
        EXPECT_EQ(sz, 3);
      }))
    .finalize();

  auto bufFrame = createZeroedBuffer(6);
  RWPrivateCursor c(bufFrame.get());
  c.write((uint16_t)100); // pre header
  c.writeBE((uint16_t)1); // frame size
  c.write((uint16_t)100); // post header
  auto bufData = createZeroedBuffer(1);

  IOBufQueue q(IOBufQueue::cacheChainLength());

  q.append(std::move(bufFrame));
  pipeline->read(q);
  EXPECT_EQ(called, 0);

  q.append(std::move(bufData));
  pipeline->read(q);
  EXPECT_EQ(called, 1);
}