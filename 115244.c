TEST(LengthFieldFramePipeline, LittleEndian) {
  auto pipeline = Pipeline<IOBufQueue&, std::unique_ptr<IOBuf>>::create();
  int called = 0;

  (*pipeline)
    .addBack(test::BytesReflector())
    .addBack(LengthFieldBasedFrameDecoder(4, 100, 0, 0, 4, false))
    .addBack(test::FrameTester([&](std::unique_ptr<IOBuf> buf) {
        auto sz = buf->computeChainDataLength();
        called++;
        EXPECT_EQ(sz, 1);
      }))
    .addBack(LengthFieldPrepender(4, 0, false, false))
    .finalize();

  auto buf = createZeroedBuffer(1);
  pipeline->write(std::move(buf));
  EXPECT_EQ(called, 1);
}