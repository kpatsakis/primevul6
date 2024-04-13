TEST(LengthFieldFramePipeline, SimpleTest) {
  auto pipeline = Pipeline<IOBufQueue&, std::unique_ptr<IOBuf>>::create();
  int called = 0;

  (*pipeline)
    .addBack(test::BytesReflector())
    .addBack(LengthFieldPrepender())
    .addBack(LengthFieldBasedFrameDecoder())
    .addBack(test::FrameTester([&](std::unique_ptr<IOBuf> buf) {
        auto sz = buf->computeChainDataLength();
        called++;
        EXPECT_EQ(sz, 2);
      }))
    .finalize();

  auto buf = createZeroedBuffer(2);
  pipeline->write(std::move(buf));
  EXPECT_EQ(called, 1);
}