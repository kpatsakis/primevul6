TEST(LineBasedFrameDecoder, CarriageOnly) {
  auto pipeline = Pipeline<IOBufQueue&, std::unique_ptr<IOBuf>>::create();

  (*pipeline)
      .addBack(LineBasedFrameDecoder(
          10, true, LineBasedFrameDecoder::TerminatorType::CARRIAGENEWLINE))
      .addBack(test::FrameTester([&](std::unique_ptr<IOBuf>) { FAIL(); }))
      .finalize();

  IOBufQueue q(IOBufQueue::cacheChainLength());
  q.append(IOBuf::copyBuffer("\raa"));
  pipeline->read(q);
}