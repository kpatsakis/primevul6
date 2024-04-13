TEST_F(AsStringGraphTest, Bool) {
  TF_ASSERT_OK(Init(DT_BOOL));

  AddInputFromArray<bool>(TensorShape({2}), {true, false});
  TF_ASSERT_OK(RunOpKernel());
  Tensor expected(allocator(), DT_STRING, TensorShape({2}));
  test::FillValues<tstring>(&expected, {"true", "false"});
  test::ExpectTensorEqual<tstring>(expected, *GetOutput(0));
}