TEST_F(AsStringGraphTest, FillWithZero) {
  TF_ASSERT_OK(Init(DT_INT64, /*fill=*/"0", /*width=*/4));

  AddInputFromArray<int64>(TensorShape({3}), {-42, 0, 42});
  TF_ASSERT_OK(RunOpKernel());
  Tensor expected(allocator(), DT_STRING, TensorShape({3}));
  test::FillValues<tstring>(&expected, {"-042", "0000", "0042"});
  test::ExpectTensorEqual<tstring>(expected, *GetOutput(0));
}