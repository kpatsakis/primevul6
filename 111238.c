TEST(TensorSliceTest, Deserialization) {
  // Serialization of
  //     extent { length: 5 }
  //     extent { start: 0 length: 10 }
  //     extent { start: 14 length: 1 }
  //     extent { start: 1 }
  //     extent { }
  // in proto2 and proto3:
  const char pb2[] =
      "\x0A\x02\x10\x05\x0A\x04\x08\x00"
      "\x10\x0A\x0A\x04\x08\x0E\x10\x01\x0A\x02\x08\x01\x0A\x00";
  const char pb3[] =
      "\x0A\x02\x10\x05\x0A\x02"
      "\x10\x0A\x0A\x04\x08\x0E\x10\x01\x0A\x02\x08\x01\x0A\x00";
  // (The difference is that in the proto3 version, "start: 0" isn't included
  // since 0 is start's default value.)

  TensorSliceProto proto2;
  ASSERT_TRUE(proto2.ParseFromArray(pb2, sizeof(pb2) - 1));
  TensorSlice ts2(proto2);

  TensorSliceProto proto3;
  ASSERT_TRUE(proto3.ParseFromArray(pb3, sizeof(pb3) - 1));
  TensorSlice ts3(proto3);

  // Both serializations should be interpreted the same.
  EXPECT_EQ("0,5:0,10:14,1:1,-1:-", ts2.DebugString());
  EXPECT_EQ("0,5:0,10:14,1:1,-1:-", ts3.DebugString());
}