  void Compute(OpKernelContext* context) override {
    const Tensor& tensor_in = context->input(0);
    const Tensor& tensor_out = context->input(1);
    const Tensor& out_backprop = context->input(2);
    OP_REQUIRES(context, tensor_in.dims() == 5,
                errors::InvalidArgument("tensor_in must be 5-dimensional"));
    OP_REQUIRES(context, tensor_out.dims() == 5,
                errors::InvalidArgument("tensor_out must be 5-dimensional"));
    OP_REQUIRES(context, out_backprop.dims() == 5,
                errors::InvalidArgument("out_backprop must be 5-dimensional"));

    const TensorShape& output_shape = tensor_in.shape();
    Tensor* input_backprop;
    OP_REQUIRES_OK(context,
                   context->allocate_output(0, output_shape, &input_backprop));
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
    std::array<int64, 3> out, padding;

    OP_REQUIRES_OK(context, Get3dOutputSize(input_size, window, stride,
                                            padding_, &out, &padding));
    LaunchMaxPooling3dGradOp<Device, T>::launch(
        context, tensor_in, tensor_out, out_backprop, window, stride, out,
        padding, data_format_, input_backprop);
  }