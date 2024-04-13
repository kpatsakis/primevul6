TEST_F(HTTP2CodecTest, Cookies) {
  HTTPMessage req = getGetRequest("/guacamole");
  req.getHeaders().add("Cookie", "chocolate-chip=1");
  req.getHeaders().add("Cookie", "rainbow-chip=2");
  req.getHeaders().add("Cookie", "butterscotch=3");
  req.getHeaders().add("Cookie", "oatmeal-raisin=4");
  req.setSecure(true);
  upstreamCodec_.generateHeader(output_, 1, req);

  parse();
  callbacks_.expectMessage(false, 2, "/guacamole");
  EXPECT_EQ(callbacks_.msg->getCookie("chocolate-chip"), "1");
  EXPECT_EQ(callbacks_.msg->getCookie("rainbow-chip"), "2");
  EXPECT_EQ(callbacks_.msg->getCookie("butterscotch"), "3");
  EXPECT_EQ(callbacks_.msg->getCookie("oatmeal-raisin"), "4");
}