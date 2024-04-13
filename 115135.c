TEST_F(AsStringGraphTest, Int64) {
  TF_ASSERT_OK(Init(DT_INT64));

  AddInputFromArray<int64>(TensorShape({3}), {-42, 0, 42});
  TF_ASSERT_OK(RunOpKernel());
  Tensor expected(allocator(), DT_STRING, TensorShape({3}));
  test::FillValues<tstring>(&expected, {"-42", "0", "42"});
  test::ExpectTensorEqual<tstring>(expected, *GetOutput(0));
}