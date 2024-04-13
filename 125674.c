TEST_F(LoaderTest, NoTagMatchMultiple) {
  SavedModelBundle bundle;
  RunOptions run_options;
  SessionOptions session_options;

  const string export_dir =
      io::JoinPath(testing::TensorFlowSrcRoot(), kTestDataSharded);
  Status st = LoadSavedModel(session_options, run_options, export_dir,
                             {kSavedModelTagServe, "missing-tag"}, &bundle);
  EXPECT_FALSE(st.ok());
  EXPECT_TRUE(absl::StrContains(
      st.error_message(),
      "Could not find meta graph def matching supplied tags: "))
      << st.error_message();
}