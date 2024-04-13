TEST_F(AsStringGraphTest, NoPrecisionForNonFloat) {
  Status s = Init(DT_INT32, /*fill=*/"", /*width=*/-1, /*precision=*/5);
  ASSERT_EQ(error::INVALID_ARGUMENT, s.code());
  ASSERT_TRUE(absl::StrContains(s.error_message(),
                                "precision not supported for datatype"));
}