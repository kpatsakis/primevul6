TEST(CStringRefTest, Ctor) {
  EXPECT_STREQ("abc", CStringRef("abc").c_str());
  EXPECT_STREQ("defg", CStringRef(std::string("defg")).c_str());

#if FMT_HAS_STRING_VIEW
  EXPECT_STREQ("hijk", CStringRef(std::string_view("hijk")).c_str());
#endif

#if FMT_HAS_EXPERIMENTAL_STRING_VIEW
  EXPECT_STREQ("hijk", CStringRef(std::experimental::string_view("hijk")).c_str());
#endif
}