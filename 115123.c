TEST_F(AsStringGraphTest, FloatWidthOnly) {
  TF_ASSERT_OK(Init(DT_FLOAT, /*fill=*/"", /*width=*/5));

  AddInputFromArray<float>(TensorShape({4}), {-42, 0, 3.14159, 42});
  TF_ASSERT_OK(RunOpKernel());
  Tensor expected(allocator(), DT_STRING, TensorShape({4}));
  test::FillValues<tstring>(
      &expected, {"-42.000000", "0.000000", "3.141590", "42.000000"});
  test::ExpectTensorEqual<tstring>(expected, *GetOutput(0));
}