TEST_F(LoaderTest, NegativeShapeDimension) {
  SavedModelBundle bundle;
  RunOptions run_options;
  SessionOptions session_options;

  const string export_dir = io::JoinPath(testing::TensorFlowSrcRoot(),
                                         kTestFuzzGeneratedNegativeShape);
  Status st = LoadSavedModel(session_options, run_options, export_dir,
                             {kSavedModelTagServe}, &bundle);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(
      st.error_message().find("initializes from a tensor with -1 elements"),
      std::string::npos);
}