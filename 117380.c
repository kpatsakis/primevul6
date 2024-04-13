  void Compute(OpKernelContext* context) override {
    const Tensor& tensor_in = context->input(0);

    OP_REQUIRES(context, tensor_in.dims() == 5,
                errors::InvalidArgument("tensor_in must be 5-dimensional"));
    const int64 depth = GetTensorDim(tensor_in, data_format_, 'C');
    const int64 in_batch = GetTensorDim(tensor_in, data_format_, 'N');

    // Dimension order for these arrays is: x, y, z.
    std::array<int64, 3> input_size{
        {GetTensorDim(tensor_in, data_format_, '2'),
         GetTensorDim(tensor_in, data_format_, '1'),
         GetTensorDim(tensor_in, data_format_, '0')}};
    std::array<int64, 3> window{{GetTensorDim(ksize_, data_format_, '2'),
                                 GetTensorDim(ksize_, data_format_, '1'),
                                 GetTensorDim(ksize_, data_format_, '0')}};
    std::array<int64, 3> stride{{GetTensorDim(stride_, data_format_, '2'),
                                 GetTensorDim(stride_, data_format_, '1'),
                                 GetTensorDim(stride_, data_format_, '0')}};
    std::array<int64, 3> padding, out;

    OP_REQUIRES_OK(context, Get3dOutputSize(input_size, window, stride,
                                            padding_, &out, &padding));

    TensorShape out_shape = ShapeFromFormat(data_format_, in_batch,
                                            {{out[2], out[1], out[0]}}, depth);
    Tensor* output;
    OP_REQUIRES_OK(context, context->allocate_output(0, out_shape, &output));
    if (out_shape.num_elements() == 0) return;
    LaunchPoolingOp<Device, T, Type>::launch(context, tensor_in, window, stride,
                                             padding, data_format_, padding_,
                                             output);
  }