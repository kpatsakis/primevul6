TEST_F(HTTP2CodecTest, StreamIdOverflow) {
  HTTP2Codec codec(TransportDirection::UPSTREAM);

  HTTPCodec::StreamID streamId;
  codec.setNextEgressStreamId(std::numeric_limits<int32_t>::max() - 10);
  while (codec.isReusable()) {
    streamId = codec.createStream();
  }
  EXPECT_EQ(streamId, std::numeric_limits<int32_t>::max() - 2);
}