TEST(FormatTest, PrintColored) {
  EXPECT_WRITE(stdout, fmt::print_colored(fmt::RED, "Hello, {}!\n", "world"),
    "\x1b[31mHello, world!\n\x1b[0m");
}