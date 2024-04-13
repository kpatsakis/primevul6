TEST(QuantizedInt8PoolingOpTest, MaxPoolActivationRelu) {
  // Choose the input ranges carefully so that the dequantized output matches
  // the results of the float model above.
  // Input Range[-15.9375, 15.8130] --> [Scale{0.124512}, zero_point{0}]
  SymmetricQuantizedPoolingOpModel m(
      BuiltinOperator_MAX_POOL_2D,
      /*input=*/{TensorType_INT8, {1, 2, 4, 1}, -15.9375, 15.8130},
      /*filter_width=*/2, /*filter_height=*/2,
      /*output=*/{TensorType_INT8, {}, -15.9375, 15.8130}, Padding_VALID, 2, 2,
      ActivationFunctionType_RELU);
  m.SetInput({
      -1.5, -6, 2, 4,  //
      -3, -2, 10, 7,   //
  });
  m.Invoke();

  EXPECT_THAT(m.GetDequantizedOutput(),
              ElementsAreArray(ArrayFloatNear({0, 10}, 0.04)));
  EXPECT_THAT(m.GetOutput(), ElementsAreArray({128 - 128, 208 - 128}));
}