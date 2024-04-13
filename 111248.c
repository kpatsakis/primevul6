TEST(TensorSliceTest, UpdateToCover) {
  // [2:4, :, 3:]
  TensorSlice s({{2, 2}, {0, -1}, {3, 7}});
  // [:, 1:4, 2:4]
  TensorSlice other({{0, -1}, {1, 3}, {2, 2}});

  s.UpdateToCover(other);
  // [:, :, 2:]
  EXPECT_EQ("-:-:2,8", s.DebugString());
}