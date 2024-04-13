TEST_F(LoaderTest, ReadMetaGraphFromSavedModel) {
  SavedModelBundle bundle;
  SessionOptions session_options;
  RunOptions run_options;

  const string export_dir =
      io::JoinPath(testing::TensorFlowSrcRoot(), kTestDataSharded);
  TF_ASSERT_OK(LoadSavedModel(session_options, run_options, export_dir,
                              {kSavedModelTagServe}, &bundle));
  MetaGraphDef actual_metagraph;
  TF_ASSERT_OK(ReadMetaGraphDefFromSavedModel(export_dir, {kSavedModelTagServe},
                                              &actual_metagraph));
  EXPECT_EQ(actual_metagraph.DebugString(),
            bundle.meta_graph_def.DebugString());
}