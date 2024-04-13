TEST(FormatterTest, FormatExperimentalStringView) {
	EXPECT_EQ("test", format("{0}", std::experimental::string_view("test")));
}