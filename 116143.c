TEST(WriterTest, MoveCtor) {
  MemoryWriter w;
  w << "test";
  check_move_writer("test", w);
  // This fills the inline buffer, but doesn't cause dynamic allocation.
  std::string s;
  for (int i = 0; i < fmt::internal::INLINE_BUFFER_SIZE; ++i)
    s += '*';
  w.clear();
  w << s;
  check_move_writer(s, w);
  const char *inline_buffer_ptr = w.data();
  // Adding one more character causes the content to move from the inline to
  // a dynamically allocated buffer.
  w << '*';
  MemoryWriter w2(std::move(w));
  // Move should rip the guts of the first writer.
  EXPECT_EQ(inline_buffer_ptr, w.data());
  EXPECT_EQ(s + '*', w2.str());
}