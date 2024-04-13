TEST_F(AsStringGraphTest, FloatScientific) {
  TF_ASSERT_OK(Init(DT_FLOAT, /*fill=*/"", /*width=*/-1, /*precision=*/-1,
                    /*scientific=*/true));

  AddInputFromArray<float>(TensorShape({4}), {-42, 0, 3.14159, 42});
  TF_ASSERT_OK(RunOpKernel());
  Tensor expected(allocator(), DT_STRING, TensorShape({4}));
  test::FillValues<tstring>(&expected, {"-4.200000e+01", "0.000000e+00",
                                        "3.141590e+00", "4.200000e+01"});
  test::ExpectTensorEqual<tstring>(expected, *GetOutput(0));
}