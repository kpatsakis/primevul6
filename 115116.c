TEST_F(AsStringGraphTest, NoScientificForNonFloat) {
  Status s = Init(DT_INT32, /*fill=*/"", /*width=*/-1, /*precision=*/-1,
                  /*scientific=*/true);
  ASSERT_EQ(error::INVALID_ARGUMENT, s.code());
  ASSERT_TRUE(absl::StrContains(
      s.error_message(),
      "scientific and shortest format not supported for datatype"));
}