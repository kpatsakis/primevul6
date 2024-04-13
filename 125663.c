TEST_F(LoaderTest, TagMatch) {
  SavedModelBundle bundle;
  SessionOptions session_options;
  RunOptions run_options;

  const string export_dir =
      io::JoinPath(testing::TensorFlowSrcRoot(), kTestDataSharded);
  TF_ASSERT_OK(LoadSavedModel(session_options, run_options, export_dir,
                              {kSavedModelTagServe}, &bundle));
  CheckSavedModelBundle(export_dir, bundle);
}