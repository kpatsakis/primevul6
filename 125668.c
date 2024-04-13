TEST_F(LoaderTest, RestoreSession) {
  SavedModelBundle bundle;
  SessionOptions session_options;
  RunOptions run_options;

  const string export_dir =
      io::JoinPath(testing::TensorFlowSrcRoot(), kTestDataSharded);
  TF_ASSERT_OK(LoadSavedModel(session_options, run_options, export_dir,
                              {kSavedModelTagServe}, &bundle));

  SavedModelBundle actual_bundle;
  const std::unordered_set<std::string> tags = {kSavedModelTagServe};
  TF_ASSERT_OK(ReadMetaGraphDefFromSavedModel(export_dir, tags,
                                              &actual_bundle.meta_graph_def));
  TF_ASSERT_OK(LoadMetagraphIntoSession(
      session_options, actual_bundle.meta_graph_def, &actual_bundle.session));
  TF_ASSERT_OK(RestoreSession(run_options, actual_bundle.meta_graph_def,
                              export_dir, &actual_bundle.session));
  CheckSavedModelBundle(export_dir, actual_bundle);
}