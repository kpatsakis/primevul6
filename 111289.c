TEST(TensorSliceTest, Basic) {
  {
    // Repeatedly setting FullSlice should work.
    TensorSlice s(3);
    EXPECT_EQ("-:-:-", s.DebugString());
    EXPECT_TRUE(s.IsFull());

    s.SetFullSlice(4);
    EXPECT_EQ("-:-:-:-", s.DebugString());
    EXPECT_TRUE(s.IsFull());
  }
}