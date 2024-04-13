TEST(TensorSliceTest, ExtentLength) {
  TensorSliceProto proto;
  // Define ptxt outside ASSERT_TRUE call to work around bug in some
  // versions of gcc that breaks when you use raw string literals
  // inside macro expansions.
  //   See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=55971
  const char* ptxt = R"pb(
    extent {}
    extent { start: 0 length: 10 }
    extent { start: 14 length: 1 }
    extent {}
  )pb";
  ASSERT_TRUE(protobuf::TextFormat::ParseFromString(ptxt, &proto));
  EXPECT_FALSE(TensorSlice::HasExtentLength(proto.extent(0)));
  EXPECT_TRUE(TensorSlice::HasExtentLength(proto.extent(1)));
  EXPECT_TRUE(TensorSlice::HasExtentLength(proto.extent(2)));
  EXPECT_FALSE(TensorSlice::HasExtentLength(proto.extent(3)));
  EXPECT_EQ(-1, TensorSlice::GetExtentLength(proto.extent(0)));
  EXPECT_EQ(10, TensorSlice::GetExtentLength(proto.extent(1)));
  EXPECT_EQ(1, TensorSlice::GetExtentLength(proto.extent(2)));
  EXPECT_EQ(-1, TensorSlice::GetExtentLength(proto.extent(3)));
}