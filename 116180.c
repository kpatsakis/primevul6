TEST(WriterTest, NotCopyConstructible) {
  EXPECT_FALSE(std::is_copy_constructible<BasicWriter<char> >::value);
}