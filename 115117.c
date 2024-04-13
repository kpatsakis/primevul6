TEST_F(AsStringGraphTest, Float_5_2_Format) {
  TF_ASSERT_OK(Init(DT_FLOAT, /*fill=*/"", /*width=*/5, /*precision=*/2));

  AddInputFromArray<float>(TensorShape({4}), {-42, 0, 3.14159, 42});
  TF_ASSERT_OK(RunOpKernel());
  Tensor expected(allocator(), DT_STRING, TensorShape({4}));
  test::FillValues<tstring>(&expected, {"-42.00", " 0.00", " 3.14", "42.00"});
  test::ExpectTensorEqual<tstring>(expected, *GetOutput(0));
}