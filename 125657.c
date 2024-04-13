TEST_F(LoaderTest, SessionCreationFailure) {
  SavedModelBundle bundle;
  // Use invalid SessionOptions to cause session creation to fail.  Default
  // options work, so provide an invalid value for the target field.
  SessionOptions session_options;
  constexpr char kInvalidTarget[] = "invalid target";
  session_options.target = kInvalidTarget;
  RunOptions run_options;

  const string export_dir =
      io::JoinPath(testing::TensorFlowSrcRoot(), kTestDataSharded);
  Status st = LoadSavedModel(session_options, run_options, export_dir,
                             {kSavedModelTagServe}, &bundle);
  EXPECT_FALSE(st.ok());
  EXPECT_TRUE(absl::StrContains(st.error_message(), kInvalidTarget))
      << st.error_message();
}