  void Compute(OpKernelContext* context) override {
    const Tensor& tensor_in_shape = context->input(0);
    const Tensor& out_backprop = context->input(1);
    OP_REQUIRES(
        context,
        tensor_in_shape.dims() == 1 && tensor_in_shape.NumElements() == 5,
        errors::InvalidArgument("tensor_in must be 1-dimensional and 5 "
                                "elements"));
    OP_REQUIRES(context, out_backprop.dims() == 5,
                errors::InvalidArgument("out_backprop must be 5-dimensional"));

    TensorShape output_shape;
    auto shape_vec = tensor_in_shape.vec<int32>();
    for (int64 i = 0; i < tensor_in_shape.NumElements(); ++i) {
      output_shape.AddDim(shape_vec(i));
    }

    Tensor* output;
    OP_REQUIRES_OK(context, context->allocate_output(0, output_shape, &output));

    // Dimension order for these arrays is x, y, z.
    std::array<int64, 3> input_size{
        {GetTensorDim(output_shape, data_format_, '2'),
         GetTensorDim(output_shape, data_format_, '1'),
         GetTensorDim(output_shape, data_format_, '0')}};
    std::array<int64, 3> window{{GetTensorDim(ksize_, data_format_, '2'),
                                 GetTensorDim(ksize_, data_format_, '1'),
                                 GetTensorDim(ksize_, data_format_, '0')}};
    std::array<int64, 3> stride{{GetTensorDim(stride_, data_format_, '2'),
                                 GetTensorDim(stride_, data_format_, '1'),
                                 GetTensorDim(stride_, data_format_, '0')}};
    std::array<int64, 3> padding, out;

    OP_REQUIRES_OK(context, Get3dOutputSize(input_size, window, stride,
                                            padding_, &out, &padding));

    LaunchAvgPooling3dGradOp<Device, T>::launch(
        context, output_shape, out_backprop, window, stride, out, padding,
        data_format_, output);
  }