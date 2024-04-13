TEST_F(AsStringGraphTest, LongFill) {
  Status s = Init(DT_INT32, /*fill=*/"asdf");
  ASSERT_EQ(error::INVALID_ARGUMENT, s.code());
  ASSERT_TRUE(absl::StrContains(s.error_message(),
                                "Fill string must be one or fewer characters"));
}