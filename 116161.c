TEST(FormatTest, Print) {
#if FMT_USE_FILE_DESCRIPTORS
  EXPECT_WRITE(stdout, fmt::print("Don't {}!", "panic"), "Don't panic!");
  EXPECT_WRITE(stderr,
      fmt::print(stderr, "Don't {}!", "panic"), "Don't panic!");
#endif
}