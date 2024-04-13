TEST_F(LoaderTest, InvalidExportPath) {
  SavedModelBundle bundle;
  RunOptions run_options;
  SessionOptions session_options;

  const string export_dir =
      io::JoinPath(testing::TensorFlowSrcRoot(), "missing-path");
  Status st = LoadSavedModel(session_options, run_options, export_dir,
                             {kSavedModelTagServe}, &bundle);
  EXPECT_FALSE(st.ok());
}