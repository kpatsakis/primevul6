  BasePoolingOpModel(
      BuiltinOperator type, const TensorData& input, int filter_width,
      int filter_height, const TensorData& output,
      Padding padding = Padding_VALID, int stride_w = 2, int stride_h = 2,
      ActivationFunctionType activation = ActivationFunctionType_NONE) {
    input_ = AddInput(input);
    output_ = AddOutput(output);

    SetBuiltinOp(type, BuiltinOptions_Pool2DOptions,
                 CreatePool2DOptions(builder_, padding, stride_w, stride_h,
                                     filter_width, filter_height, activation)
                     .Union());

    BuildInterpreter({GetShape(input_)});
  }