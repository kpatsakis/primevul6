  void Compute(OpKernelContext* context) override {
    const Tensor& input = context->input(0);
    const Tensor& filter_sizes = context->input(1);
    const Tensor& out_backprop = context->input(2);
    OP_REQUIRES(
        context, TensorShapeUtils::IsVector(filter_sizes.shape()),
        errors::InvalidArgument(
            "Conv2DBackpropFilter: filter_sizes input must be 1-dim, not ",
            filter_sizes.dims()));
    TensorShape filter_shape;
    OP_REQUIRES_OK(context, TensorShapeUtils::MakeShape(
                                filter_sizes.vec<int32>(), &filter_shape));

    Tensor* filter_backprop = nullptr;
    OP_REQUIRES_OK(context,
                   context->allocate_output(0, filter_shape, &filter_backprop));

    // If there is nothing to compute, return.
    if (filter_shape.num_elements() == 0) {
      return;
    }
    // If input is empty, set gradients to zero.
    if (input.shape().num_elements() == 0) {
      functor::SetZeroFunctor<Device, T> f;
      f(context->eigen_device<Device>(), filter_backprop->flat<T>());
      return;
    }

    // For now we take the stride from the second and third dimensions only (we
    // do not support striding on the batch or depth dimension).
    const int stride_rows = GetTensorDim(strides_, data_format_, 'H');
    const int stride_cols = GetTensorDim(strides_, data_format_, 'W');
    const int dilation_rows = GetTensorDim(dilations_, data_format_, 'H');
    const int dilation_cols = GetTensorDim(dilations_, data_format_, 'W');

    VLOG(2) << "Conv2DBackpropFilter:"
            << " input: " << input.shape().DebugString()
            << " filter:" << filter_shape.DebugString()
            << " out_backprop: " << out_backprop.shape().DebugString()
            << " strides: [" << stride_rows << ", " << stride_cols << "]"
            << " dilations: [" << dilation_rows << ", " << dilation_cols << "]";

    launcher_(context, use_cudnn_, cudnn_use_autotune_, out_backprop, input,
              dilation_rows, dilation_cols, stride_rows, stride_cols, padding_,
              explicit_paddings_, filter_backprop, data_format_);
  }