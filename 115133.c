TEST_F(AsStringGraphTest, String) {
  Status s = Init(DT_STRING);
  ASSERT_EQ(error::INVALID_ARGUMENT, s.code());
  ASSERT_TRUE(absl::StrContains(
      s.error_message(),
      "Value for attr 'T' of string is not in the list of allowed values"));
}