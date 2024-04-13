TEST_F(AsStringGraphTest, FillWithChar4) {
  Status s = Init(DT_INT32, /*fill=*/"n");
  ASSERT_EQ(error::INVALID_ARGUMENT, s.code());
  ASSERT_TRUE(
      absl::StrContains(s.error_message(), "Fill argument not supported"));
}