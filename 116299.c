TEST(WriterTest, NotCopyAssignable) {
  EXPECT_FALSE(std::is_copy_assignable<BasicWriter<char> >::value);
}