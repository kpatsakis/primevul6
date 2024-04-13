TEST(LineBasedFrameDecoder, Fail) {
  auto pipeline = Pipeline<IOBufQueue&, std::unique_ptr<IOBuf>>::create();
  int called = 0;

  (*pipeline)
    .addBack(LineBasedFrameDecoder(10))
    .addBack(test::FrameTester([&](std::unique_ptr<IOBuf> buf) {
        ASSERT_EQ(nullptr, buf);
        called++;
      }))
    .finalize();

  auto buf = createZeroedBuffer(11);

  IOBufQueue q(IOBufQueue::cacheChainLength());

  q.append(std::move(buf));
  pipeline->read(q);
  EXPECT_EQ(called, 1);

  buf = createZeroedBuffer(1);
  q.append(std::move(buf));
  pipeline->read(q);
  EXPECT_EQ(called, 1);

  buf = createZeroedBuffer(2);
  RWPrivateCursor c(buf.get());
  c.write(' ');
  c.write<char>('\n');
  q.append(std::move(buf));
  pipeline->read(q);
  EXPECT_EQ(called, 1);

  buf = createZeroedBuffer(12);
  RWPrivateCursor c2(buf.get());
  for (int i = 0; i < 11; i++) {
    c2.write(' ');
  }
  c2.write<char>('\n');
  q.append(std::move(buf));
  pipeline->read(q);
  EXPECT_EQ(called, 2);
}