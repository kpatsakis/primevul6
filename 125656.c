TEST_F(LoaderTest, SavedModelInitOpV2Format) {
  SavedModelBundle bundle;
  SessionOptions session_options;
  RunOptions run_options;

  const string export_dir =
      io::JoinPath(testing::TensorFlowSrcRoot(), kTestDataInitOpV2);
  TF_ASSERT_OK(LoadSavedModel(session_options, run_options, export_dir,
                              {kSavedModelTagServe}, &bundle));
  CheckSavedModelBundle(export_dir, bundle);
}