TEST(StringRefTest, Ctor) {
  EXPECT_STREQ("abc", StringRef("abc").data());
  EXPECT_EQ(3u, StringRef("abc").size());

  EXPECT_STREQ("defg", StringRef(std::string("defg")).data());
  EXPECT_EQ(4u, StringRef(std::string("defg")).size());

#if FMT_HAS_STRING_VIEW
  EXPECT_STREQ("hijk", StringRef(std::string_view("hijk")).data());
  EXPECT_EQ(4u, StringRef(std::string_view("hijk")).size());
#endif

#if FMT_HAS_EXPERIMENTAL_STRING_VIEW
  EXPECT_STREQ("hijk", StringRef(std::experimental::string_view("hijk")).data());
  EXPECT_EQ(4u, StringRef(std::experimental::string_view("hijk")).size());
#endif
}