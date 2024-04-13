TEST(FormatTest, MaxArgs) {
  EXPECT_EQ("0123456789abcde",
            fmt::format("{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}",
                        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 'a', 'b', 'c', 'd', 'e'));
}