TEST(QuantizedPoolingOpTest, AveragePool) {
  // Choose the input ranges carefully so that the dequantized output matches
  // the results of the float model above.
  // Input Range[0, 15.9375] --> [Scale{0.0625}, zero_point{0}]
  QuantizedPoolingOpModel m(
      BuiltinOperator_AVERAGE_POOL_2D,
      /*input=*/{TensorType_UINT8, {1, 2, 4, 1}, 0, 15.9375},
      /*filter_width=*/2, /*filter_height=*/2,
      /*output=*/{TensorType_UINT8, {}, 0, 15.9375});
  m.SetInput({
      0, 6, 2, 4,   //
      3, 2, 10, 7,  //
  });
  m.Invoke();

  EXPECT_THAT(m.GetDequantizedOutput(),
              ElementsAreArray(ArrayFloatNear({2.75, 5.75})));
  EXPECT_THAT(m.GetOutput(), ElementsAreArray({44, 92}));
}