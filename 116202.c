TEST(FormatTest, Regression) {
  fmt::format("...........{:<77777.7p}", "foo");
}