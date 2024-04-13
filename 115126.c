TEST_F(AsStringGraphTest, FillWithChar3) {
  Status s = Init(DT_INT32, /*fill=*/"s");
  ASSERT_EQ(error::INVALID_ARGUMENT, s.code());
  ASSERT_TRUE(
      absl::StrContains(s.error_message(), "Fill argument not supported"));
}