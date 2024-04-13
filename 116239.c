TEST(WriterTest, Allocator) {
  typedef testing::StrictMock< MockAllocator<char> > MockAllocator;
  typedef AllocatorRef<MockAllocator> TestAllocator;
  MockAllocator alloc;
  fmt::BasicMemoryWriter<char, TestAllocator> w((TestAllocator(&alloc)));
  std::size_t size =
      static_cast<std::size_t>(1.5 * fmt::internal::INLINE_BUFFER_SIZE);
  std::vector<char> mem(size);
  EXPECT_CALL(alloc, allocate(size, 0)).WillOnce(testing::Return(&mem[0]));
  for (int i = 0; i < fmt::internal::INLINE_BUFFER_SIZE + 1; ++i)
    w << '*';
  EXPECT_CALL(alloc, deallocate(&mem[0], size));
}