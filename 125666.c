TEST_F(LoaderTest, ResourceLeakTest) {
  SavedModelBundle bundle;
  SessionOptions session_options;
  RunOptions run_options;

  const string export_dir =
      io::JoinPath(testing::TensorFlowSrcRoot(), kTestDataSharded);
  for (int i = 0; i < 100; ++i) {
    TF_ASSERT_OK(LoadSavedModel(session_options, run_options, export_dir,
                                {kSavedModelTagServe}, &bundle));
    CheckSavedModelBundle(export_dir, bundle);
  }
}