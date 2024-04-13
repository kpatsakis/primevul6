void CachedTensorSliceReaderTesterHelper(
    const TensorSliceWriter::CreateBuilderFunction& create_function,
    const TensorSliceReader::OpenTableFunction& open_function) {
  const string fname_base = io::JoinPath(testing::TmpDir(), "float_checkpoint");

  TensorShape shape({4, 5});

  // File #0 contains a slice that is the top two rows:
  //
  //   0   1   2   3   4
  //   5   6   7   8   9
  //   .   .   .   .   .
  //   .   .   .   .   .
  {
    const string fname = strings::StrCat(fname_base, "_0");
    TensorSliceWriter writer(fname, create_function);
    const float data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    TensorSlice slice = TensorSlice::ParseOrDie("0,2:-");
    TF_CHECK_OK(writer.Add("test", shape, slice, data));
    TF_CHECK_OK(writer.Finish());
  }

  // File #1 contains two slices:
  //
  // slice #0 is the bottom left corner
  //   .   .   .   .   .
  //   .   .   .   .   .
  //  10  11  12   .   .
  //  15  16  17   .   .
  //
  // slice #1 is the bottom right corner
  //   .   .   .   .   .
  //   .   .   .   .   .
  //   .   .   .   .   .
  //   .   .   .  18  19
  {
    const string fname = strings::StrCat(fname_base, "_1");
    TensorSliceWriter writer(fname, create_function);
    // slice #0
    {
      const float data[] = {10, 11, 12, 15, 16, 17};
      TensorSlice slice = TensorSlice::ParseOrDie("2,2:0,3");
      TF_CHECK_OK(writer.Add("test", shape, slice, data));
    }
    // slice #1
    {
      const float data[] = {18, 19};
      TensorSlice slice = TensorSlice::ParseOrDie("3,1:3,2");
      TF_CHECK_OK(writer.Add("test", shape, slice, data));
    }
    TF_CHECK_OK(writer.Finish());
  }

  // Notice that we leave a hole in the tensor
  //   .   .   .   .   .
  //   .   .   .   .   .
  //   .   .   . (13) (14)
  //   .   .   .   .   .

  // Now we need to read the tensor slices
  TensorSliceReaderCache cache;
  const string filepattern = strings::StrCat(fname_base, "_*");
  const TensorSliceReader* reader = cache.GetReader(
      filepattern, open_function, TensorSliceReader::kLoadAllShards);
  EXPECT_TRUE(reader != nullptr);
  EXPECT_EQ(2, reader->num_files());

  // We query some of the tensors
  {
    TensorShape shape;
    DataType type;
    EXPECT_TRUE(reader->HasTensor("test", &shape, &type));
    EXPECT_EQ("[4,5]", shape.DebugString());
    EXPECT_EQ(DT_FLOAT, type);
    EXPECT_FALSE(reader->HasTensor("don't exist", nullptr, nullptr));
  }

  // Make sure the reader is cached.
  const TensorSliceReader* reader2 = cache.GetReader(
      filepattern, open_function, TensorSliceReader::kLoadAllShards);
  EXPECT_EQ(reader, reader2);

  reader = cache.GetReader("file_does_not_exist", open_function,
                           TensorSliceReader::kLoadAllShards);
  EXPECT_TRUE(reader == nullptr);
}