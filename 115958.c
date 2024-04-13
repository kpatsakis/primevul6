TEST(FormatterTest, ArgsInDifferentPositions) {
  EXPECT_EQ("42", format("{0}", 42));
  EXPECT_EQ("before 42", format("before {0}", 42));
  EXPECT_EQ("42 after", format("{0} after", 42));
  EXPECT_EQ("before 42 after", format("before {0} after", 42));
  EXPECT_EQ("answer = 42", format("{0} = {1}", "answer", 42));
  EXPECT_EQ("42 is the answer", format("{1} is the {0}", "answer", 42));
  EXPECT_EQ("abracadabra", format("{0}{1}{0}", "abra", "cad"));
}