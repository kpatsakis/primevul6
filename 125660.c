TEST_F(LoaderTest, MaybeSavedModelDirectory) {
  // Valid SavedModel directory.
  const string export_dir =
      io::JoinPath(testing::TensorFlowSrcRoot(), kTestDataSharded);
  EXPECT_TRUE(MaybeSavedModelDirectory(export_dir));

  // Directory that does not exist.
  const string missing_export_dir =
      io::JoinPath(testing::TensorFlowSrcRoot(), "missing-path");
  EXPECT_FALSE(MaybeSavedModelDirectory(missing_export_dir));

  // Directory that exists but is an invalid SavedModel location.
  const string invalid_export_dir =
      io::JoinPath(testing::TensorFlowSrcRoot(), "cc/saved_model");
  EXPECT_FALSE(MaybeSavedModelDirectory(invalid_export_dir));
}