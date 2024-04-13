TEST_F(LoaderTest, SavedModelV2DebugInfo) {
  SavedModelBundle bundle;
  SessionOptions session_options;
  RunOptions run_options;

  const string export_dir =
      io::JoinPath(testing::TensorFlowSrcRoot(), kTestDataV2DebugInfo);
  TF_ASSERT_OK(LoadSavedModel(session_options, run_options, export_dir,
                              {kSavedModelTagServe}, &bundle));

  // This SavedModel has debug info, so we should have loaded it.
  EXPECT_NE(bundle.debug_info.get(), nullptr);
}