TEST_F(AsStringGraphTest, Int8) {
  TF_ASSERT_OK(Init(DT_INT8));

  AddInputFromArray<int8>(TensorShape({3}), {-42, 0, 42});
  TF_ASSERT_OK(RunOpKernel());
  Tensor expected(allocator(), DT_STRING, TensorShape({3}));
  test::FillValues<tstring>(&expected, {"-42", "0", "42"});
  test::ExpectTensorEqual<tstring>(expected, *GetOutput(0));
}