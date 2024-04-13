TEST(QuantizedUInt8PoolingOpTest, MaxPoolActivationRelu1) {
  // Choose the input ranges carefully so that the dequantized output matches
  // the results of the float model above.
  // Input Range[-15.9375, 15.9375] --> [Scale{0.125}, zero_point{128}]
  QuantizedPoolingOpModel m(
      BuiltinOperator_MAX_POOL_2D,
      /*input=*/{TensorType_UINT8, {1, 2, 4, 1}, -15.9375, 15.9375},
      /*filter_width=*/2, /*filter_height=*/2,
      /*output=*/{TensorType_UINT8, {}, -15.9375, 15.9375}, Padding_VALID, 2, 2,
      ActivationFunctionType_RELU_N1_TO_1);
  m.SetInput({
      -1.7, -6, 2, 4,  //
      -3, -2, -10, 7,  //
  });
  m.Invoke();

  EXPECT_THAT(m.GetDequantizedOutput(),
              ElementsAreArray(ArrayFloatNear({-1.0, 1.0})));
  EXPECT_THAT(m.GetOutput(), ElementsAreArray({120, 136}));

  m.SetInput({
      0, -6, -0.2, -0.4,    //
      -3, -2, 0.75, -0.99,  //
  });
  m.Invoke();

  EXPECT_THAT(m.GetDequantizedOutput(),
              ElementsAreArray(ArrayFloatNear({0.0, 0.75})));
  EXPECT_THAT(m.GetOutput(), ElementsAreArray({128, 134}));
}