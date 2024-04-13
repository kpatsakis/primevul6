  void CheckSavedModelBundle(const string& export_dir,
                             const SavedModelBundle& bundle) {
    ValidateAssets(export_dir, bundle);
    // Retrieve the regression signature from meta graph def.
    const auto& signature_def = bundle.GetSignatures().at("regress_x_to_y");

    const string input_name = signature_def.inputs().at(kRegressInputs).name();
    const string output_name =
        signature_def.outputs().at(kRegressOutputs).name();

    std::vector<tstring> serialized_examples;
    for (float x : {0, 1, 2, 3}) {
      serialized_examples.push_back(MakeSerializedExample(x));
    }

    // Validate the half plus two behavior.
    Tensor input =
        test::AsTensor<tstring>(serialized_examples, TensorShape({4}));
    std::vector<Tensor> outputs;
    TF_ASSERT_OK(bundle.session->Run({{input_name, input}}, {output_name}, {},
                                     &outputs));
    ASSERT_EQ(outputs.size(), 1);
    test::ExpectTensorEqual<float>(
        outputs[0],
        test::AsTensor<float>({2, 2.5, 3, 3.5}, TensorShape({4, 1})));
  }