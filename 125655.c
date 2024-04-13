  void ValidateAssets(const string& export_dir,
                      const SavedModelBundle& bundle) {
    const string asset_directory =
        io::JoinPath(export_dir, kSavedModelAssetsDirectory);
    const string asset_filename = "foo.txt";
    const string asset_filepath = io::JoinPath(asset_directory, asset_filename);
    TF_EXPECT_OK(Env::Default()->FileExists(asset_filepath));

    std::vector<Tensor> path_outputs;
    TF_ASSERT_OK(
        bundle.session->Run({}, {"filename_tensor:0"}, {}, &path_outputs));
    ASSERT_EQ(1, path_outputs.size());

    test::ExpectTensorEqual<tstring>(
        test::AsTensor<tstring>({"foo.txt"}, TensorShape({})), path_outputs[0]);
  }