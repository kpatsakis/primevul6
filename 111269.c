TEST(TensorSliceTest, IsFull) {
  TensorSlice slice(3);
  EXPECT_TRUE(slice.IsFull());

  TensorSlice slice2({{0, -1}});
  EXPECT_TRUE(slice2.IsFull());

  TensorSlice slice3({{0, -1}, {0, -1}, {14, 1}});
  EXPECT_TRUE(!slice3.IsFull());
}