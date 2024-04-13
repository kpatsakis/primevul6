TEST_F(LoaderTest, BadNodeAttr) {
  SavedModelBundle bundle;
  RunOptions run_options;
  SessionOptions session_options;

  const string export_dir =
      io::JoinPath(testing::TensorFlowSrcRoot(), kTestFuzzGeneratedBadNodeAttr);
  Status st = LoadSavedModel(session_options, run_options, export_dir,
                             {kSavedModelTagServe}, &bundle);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(
      st.error_message().find("constant tensor but no value has been provided"),
      std::string::npos);
}