TEST(HTTP2CodecConstantsTest, HTTPContantsAreCommonHeaders) {
  // The purpose of this test is to verify some basic assumptions that should
  // never change but to make clear that the following http2 header constants
  // map to the respective common headers.  Should this test ever fail, the
  // H2Codec would need to be updated in the corresponding places when creating
  // compress/Header objects.
  EXPECT_EQ(HTTPCommonHeaders::hash(headers::kMethod),
            HTTP_HEADER_COLON_METHOD);
  EXPECT_EQ(HTTPCommonHeaders::hash(headers::kScheme),
            HTTP_HEADER_COLON_SCHEME);
  EXPECT_EQ(HTTPCommonHeaders::hash(headers::kPath),
            HTTP_HEADER_COLON_PATH);
  EXPECT_EQ(
    HTTPCommonHeaders::hash(headers::kAuthority),
    HTTP_HEADER_COLON_AUTHORITY);
  EXPECT_EQ(HTTPCommonHeaders::hash(headers::kStatus),
            HTTP_HEADER_COLON_STATUS);
}