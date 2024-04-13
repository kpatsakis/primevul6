TEST(TensorSliceTest, ComputeRelative) {
  // Easy case: base is "everything"
  {
    TensorSlice base = TensorSlice::ParseOrDie("-:-:-:-");
    TensorSlice sub = TensorSlice::ParseOrDie("-:1,2:-:3,4");
    TensorSlice relative;
    base.ComputeRelative(sub, &relative);
    EXPECT_EQ("-:1,2:-:3,4", relative.DebugString());
  }

  // A slightly more complicated case
  {
    TensorSlice base = TensorSlice::ParseOrDie("1,2:3,4:-:5,1");
    TensorSlice sub = TensorSlice::ParseOrDie("1,1:4,2:3,3:5,1");
    TensorSlice relative;
    base.ComputeRelative(sub, &relative);
    EXPECT_EQ("0,1:1,2:3,3:0,1", relative.DebugString());
  }
}