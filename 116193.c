TEST(StringRefTest, ConvertToString) {
  std::string s = StringRef("abc").to_string();
  EXPECT_EQ("abc", s);

#if FMT_HAS_STRING_VIEW
  StringRef str_ref("defg");
  std::string_view sv = static_cast<std::string_view>(str_ref);
  EXPECT_EQ("defg", sv);
#endif

#if FMT_HAS_EXPERIMENTAL_STRING_VIEW
  StringRef str_ref("defg");
  std::experimental::string_view sv = static_cast<std::experimental::string_view>(str_ref);
  EXPECT_EQ("defg", sv);
#endif
}