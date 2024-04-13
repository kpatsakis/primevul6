void check_move_writer(const std::string &str, MemoryWriter &w) {
  MemoryWriter w2(std::move(w));
  // Move shouldn't destroy the inline content of the first writer.
  EXPECT_EQ(str, w.str());
  EXPECT_EQ(str, w2.str());
}