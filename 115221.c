TEST(LengthFieldFrameDecoder, FailTestLengthFieldFrameSize) {
  auto pipeline = Pipeline<IOBufQueue&, std::unique_ptr<IOBuf>>::create();
  int called = 0;

  (*pipeline)
    .addBack(LengthFieldBasedFrameDecoder(4, 10, 0, 0, 4))
    .addBack(test::FrameTester([&](std::unique_ptr<IOBuf> buf) {
        ASSERT_EQ(nullptr, buf);
        called++;
      }))
    .finalize();

  auto bufFrame = createZeroedBuffer(16);
  RWPrivateCursor c(bufFrame.get());
  c.writeBE((uint32_t)12); // frame size
  c.write((uint32_t)0); // nothing
  c.write((uint32_t)0); // nothing
  c.write((uint32_t)0); // nothing

  IOBufQueue q(IOBufQueue::cacheChainLength());

  q.append(std::move(bufFrame));
  pipeline->read(q);
  EXPECT_EQ(called, 1);
}