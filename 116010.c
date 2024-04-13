TEST(FormatTest, JoinArg) {
  using fmt::join;
  int v1[3] = { 1, 2, 3 };
  std::vector<float> v2;
  v2.push_back(1.2f);
  v2.push_back(3.4f);

  EXPECT_EQ("(1, 2, 3)", format("({})", join(v1, v1 + 3, ", ")));
  EXPECT_EQ("(1)", format("({})", join(v1, v1 + 1, ", ")));
  EXPECT_EQ("()", format("({})", join(v1, v1, ", ")));
  EXPECT_EQ("(001, 002, 003)", format("({:03})", join(v1, v1 + 3, ", ")));
  EXPECT_EQ("(+01.20, +03.40)",
            format("({:+06.2f})", join(v2.begin(), v2.end(), ", ")));

  EXPECT_EQ(L"(1, 2, 3)", format(L"({})", join(v1, v1 + 3, L", ")));
  EXPECT_EQ("1, 2, 3", format("{0:{1}}", join(v1, v1 + 3, ", "), 1)); 

#if FMT_HAS_GXX_CXX11
  EXPECT_EQ("(1, 2, 3)", format("({})", join(v1, ", ")));
  EXPECT_EQ("(+01.20, +03.40)", format("({:+06.2f})", join(v2, ", ")));
#endif
}