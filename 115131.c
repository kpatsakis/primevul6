TEST_F(AsStringGraphTest, FloatShortest) {
  TF_ASSERT_OK(Init(DT_FLOAT, /*fill=*/"", /*width=*/-1, /*precision=*/-1,
                    /*scientific=*/false, /*shortest=*/true));

  AddInputFromArray<float>(TensorShape({4}), {-42, 0, 3.14159, 42});
  TF_ASSERT_OK(RunOpKernel());
  Tensor expected(allocator(), DT_STRING, TensorShape({4}));
  test::FillValues<tstring>(&expected, {"-42", "0", "3.14159", "42"});
  test::ExpectTensorEqual<tstring>(expected, *GetOutput(0));
}