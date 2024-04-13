TEST_F(HTTP2CodecTest, FrameTooLarge) {
  writeFrameHeaderManual(output_, 1 << 15, 0, 0, 1);

  parse();
  EXPECT_EQ(callbacks_.messageBegin, 0);
  EXPECT_EQ(callbacks_.headersComplete, 0);
  EXPECT_EQ(callbacks_.messageComplete, 0);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 1);
  EXPECT_TRUE(callbacks_.lastParseError->hasCodecStatusCode());
  EXPECT_EQ(callbacks_.lastParseError->getCodecStatusCode(),
            ErrorCode::FRAME_SIZE_ERROR);
}