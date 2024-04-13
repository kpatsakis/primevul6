TEST(TensorSliceTest, Serialization) {
  // Serialization
  {
    TensorSlice s({{0, -1}, {0, 10}, {14, 1}, {0, -1}});
    EXPECT_EQ("-:0,10:14,1:-", s.DebugString());
    EXPECT_TRUE(!s.IsFull());
  }

  {
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
    TensorSlice s(proto);
    EXPECT_EQ("-:0,10:14,1:-", s.DebugString());
    EXPECT_TRUE(!s.IsFull());
  }

  // Parsing
  {
    TensorSlice s = TensorSlice::ParseOrDie("-:-:1,3:4,5");
    TensorSliceProto proto;
    s.AsProto(&proto);
    EXPECT_EQ(
        "extent { } "
        "extent { } "
        "extent { start: 1 length: 3 } "
        "extent { start: 4 length: 5 }",
        proto.ShortDebugString());
    EXPECT_TRUE(!s.IsFull());
  }

  // Failed parsing
  {
    TensorSlice slice;
    Status s = TensorSlice::Parse("-:-:1,3:4:5", &slice);
    EXPECT_EQ(s.code(), error::INVALID_ARGUMENT);
    EXPECT_TRUE(
        absl::StrContains(s.error_message(),
                          "Expected a pair of numbers or '-' but got '4': "
                          "string = -:-:1,3:4:5"));
  }
  {
    TensorSlice slice;
    Status s = TensorSlice::Parse("-:-1,3", &slice);
    EXPECT_EQ(s.code(), error::INVALID_ARGUMENT);
    EXPECT_TRUE(absl::StrContains(
        s.error_message(),
        "Expected non-negative start and positive length but got "
        "start = -1, length = 3: string = -:-1,3"));
  }

  // int64 parsing
  {
    TensorSlice s =
        TensorSlice::ParseOrDie("9223372036854775807,9223372036854775807");
    TensorSliceProto proto;
    s.AsProto(&proto);
    EXPECT_EQ(
        "extent { start: 9223372036854775807 length: 9223372036854775807 }",
        proto.ShortDebugString());
    EXPECT_TRUE(!s.IsFull());
  }

  // int64 parsing failure
  {
    TensorSlice slice;
    Status s =
        TensorSlice::Parse("19223372036854775808,19223372036854775808", &slice);
    EXPECT_EQ(s.code(), error::INVALID_ARGUMENT);
    EXPECT_TRUE(absl::StrContains(
        s.error_message(),
        "Expected a pair of numbers or '-' but got "
        "'19223372036854775808,19223372036854775808': string = "
        "19223372036854775808,19223372036854775808"));
  }
}