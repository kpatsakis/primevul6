TEST(FormatIntTest, Data) {
  fmt::FormatInt format_int(42);
  EXPECT_EQ("42", std::string(format_int.data(), format_int.size()));
}