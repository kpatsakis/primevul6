TEST(LiteralsTest, NamedArg) {
  auto udl_a = format("{first}{second}{first}{third}",
                      "first"_a="abra", "second"_a="cad", "third"_a=99);
  EXPECT_EQ(format("{first}{second}{first}{third}",
                   fmt::arg("first", "abra"), fmt::arg("second", "cad"),
                   fmt::arg("third", 99)),
            udl_a);
  auto udl_a_w = format(L"{first}{second}{first}{third}",
                        L"first"_a=L"abra", L"second"_a=L"cad", L"third"_a=99);
  EXPECT_EQ(format(L"{first}{second}{first}{third}",
                   fmt::arg(L"first", L"abra"), fmt::arg(L"second", L"cad"),
                   fmt::arg(L"third", 99)),
            udl_a_w);
}