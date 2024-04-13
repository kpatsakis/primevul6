TEST(LineBasedFrameDecoder, SaveDelimiter) {
  auto pipeline = Pipeline<IOBufQueue&, std::unique_ptr<IOBuf>>::create();
  int called = 0;

  (*pipeline)
    .addBack(LineBasedFrameDecoder(10, false))
    .addBack(test::FrameTester([&](std::unique_ptr<IOBuf> buf) {
        auto sz = buf->computeChainDataLength();
        called++;
        EXPECT_EQ(sz, 4);
      }))
    .finalize();

  auto buf = createZeroedBuffer(3);

  IOBufQueue q(IOBufQueue::cacheChainLength());

  q.append(std::move(buf));
  pipeline->read(q);
  EXPECT_EQ(called, 0);

  buf = createZeroedBuffer(1);
  RWPrivateCursor c(buf.get());
  c.write<char>('\n');
  q.append(std::move(buf));
  pipeline->read(q);
  EXPECT_EQ(called, 1);

  buf = createZeroedBuffer(3);
  RWPrivateCursor c1(buf.get());
  c1.write(' ');
  c1.write(' ');
  c1.write('\r');
  q.append(std::move(buf));
  pipeline->read(q);
  EXPECT_EQ(called, 1);

  buf = createZeroedBuffer(1);
  RWPrivateCursor c2(buf.get());
  c2.write('\n');
  q.append(std::move(buf));
  pipeline->read(q);
  EXPECT_EQ(called, 2);
}