  void operator()(OpKernelContext* context, const Tensor& y_backprop,
                  const Tensor& x, const Tensor& scale, const Tensor& mean,
                  const Tensor& inv_variance, U epsilon, Tensor* x_backprop,
                  Tensor* scale_backprop, Tensor* offset_backprop,
                  bool use_reserved_space, TensorFormat tensor_format) {
    auto* stream = context->op_device_context()->stream();
    OP_REQUIRES(context, stream, errors::Internal("No GPU stream available"));

    const int64 batch_size = GetTensorDim(x, tensor_format, 'N');
    const int64 channels = GetTensorDim(x, tensor_format, 'C');
    const int64 height = GetTensorDim(x, tensor_format, 'H');
    const int64 width = GetTensorDim(x, tensor_format, 'W');

#if GOOGLE_CUDA
    // Check if cuDNN batch normalization has a fast NHWC implementation:
    //   (1) Tensorflow enabled batchnorm spatial persistence, and
    //       FusedBatchNormGradV3 passed non-null reserve space and allocator.
    const bool fast_nhwc_batch_norm = BatchnormSpatialPersistentEnabled() &&
                                      DataTypeToEnum<T>::value == DT_HALF &&
                                      use_reserved_space;
#else
    // fast NHWC implementation is a CUDA only feature
    const bool fast_nhwc_batch_norm = false;
#endif

    // If input tensor is in NHWC format, and we have a fast cuDNN
    // implementation, there is no need to do data format conversion.
    TensorFormat compute_format =
        fast_nhwc_batch_norm && tensor_format == FORMAT_NHWC ? FORMAT_NHWC
                                                             : FORMAT_NCHW;

    VLOG(2) << "FusedBatchNormGrad:"
            << " batch_size: " << batch_size << " channels: " << channels
            << " height: " << height << " width: " << width
            << " y_backprop shape: " << y_backprop.shape().DebugString()
            << " x shape: " << x.shape().DebugString()
            << " scale shape: " << scale.shape().DebugString()
            << " tensor format: " << ToString(tensor_format)
            << " compute format: " << ToString(compute_format);

    // Inputs
    Tensor y_backprop_maybe_transformed = y_backprop;
    Tensor x_maybe_transformed = x;
    Tensor y_backprop_transformed;
    Tensor x_transformed;

    // Outputs
    Tensor x_backprop_transformed;
    se::DeviceMemory<T> x_backprop_ptr;

    if (tensor_format == compute_format) {
      x_backprop_ptr = StreamExecutorUtil::AsDeviceMemory<T>(*x_backprop);
    } else if (tensor_format == FORMAT_NHWC && compute_format == FORMAT_NCHW) {
      // Transform inputs from 'NHWC' to 'NCHW'
      OP_REQUIRES_OK(context, context->allocate_temp(
                                  DataTypeToEnum<T>::value,
                                  ShapeFromFormat(FORMAT_NCHW, batch_size,
                                                  height, width, channels),
                                  &y_backprop_transformed));
      functor::NHWCToNCHW<GPUDevice, T, 4>()(
          context->eigen_device<GPUDevice>(),
          const_cast<const Tensor&>(y_backprop_maybe_transformed)
              .tensor<T, 4>(),
          y_backprop_transformed.tensor<T, 4>());
      y_backprop_maybe_transformed = y_backprop_transformed;

      OP_REQUIRES_OK(context, context->allocate_temp(
                                  DataTypeToEnum<T>::value,
                                  ShapeFromFormat(FORMAT_NCHW, batch_size,
                                                  height, width, channels),
                                  &x_transformed));
      functor::NHWCToNCHW<GPUDevice, T, 4>()(
          context->eigen_device<GPUDevice>(),
          const_cast<const Tensor&>(x_maybe_transformed).tensor<T, 4>(),
          x_transformed.tensor<T, 4>());
      x_maybe_transformed = x_transformed;

      // Allocate memory for transformed outputs in 'NCHW'
      OP_REQUIRES_OK(context, context->allocate_temp(
                                  DataTypeToEnum<T>::value,
                                  ShapeFromFormat(FORMAT_NCHW, batch_size,
                                                  height, width, channels),
                                  &x_backprop_transformed));
      x_backprop_ptr =
          StreamExecutorUtil::AsDeviceMemory<T>(x_backprop_transformed);
    } else {
      context->SetStatus(errors::Internal(
          "Unsupported tensor format: ", ToString(tensor_format),
          " and compute format: ", ToString(compute_format)));
      return;
    }

    const se::dnn::DataLayout data_layout =
        compute_format == FORMAT_NHWC ? se::dnn::DataLayout::kBatchYXDepth
                                      : se::dnn::DataLayout::kBatchDepthYX;

    se::dnn::BatchDescriptor x_desc;
    x_desc.set_count(batch_size)
        .set_feature_map_count(channels)
        .set_height(height)
        .set_width(width)
        .set_layout(data_layout);

    se::dnn::BatchDescriptor scale_offset_desc;
    scale_offset_desc.set_count(1)
        .set_feature_map_count(channels)
        .set_height(1)
        .set_width(1)
        .set_layout(se::dnn::DataLayout::kBatchDepthYX);

    auto y_backprop_ptr =
        StreamExecutorUtil::AsDeviceMemory<T>(y_backprop_maybe_transformed);
    auto x_ptr = StreamExecutorUtil::AsDeviceMemory<T>(x_maybe_transformed);
    auto scale_ptr = StreamExecutorUtil::AsDeviceMemory<U>(scale);
    auto mean_ptr = StreamExecutorUtil::AsDeviceMemory<U>(mean);
    auto inv_variance_ptr = StreamExecutorUtil::AsDeviceMemory<U>(inv_variance);
    auto scale_backprop_ptr =
        StreamExecutorUtil::AsDeviceMemory<U>(*scale_backprop);
    auto offset_backprop_ptr =
        StreamExecutorUtil::AsDeviceMemory<U>(*offset_backprop);

    std::unique_ptr<functor::CudnnBatchNormAllocatorInTemp<uint8>>
        workspace_allocator;
    DeviceMemory<uint8>* reserve_space_data_ptr = nullptr;
    DeviceMemory<uint8> reserve_space_data;
#if CUDNN_VERSION >= 7402
    if (use_reserved_space) {
      const Tensor& reserve_space = context->input(5);
      workspace_allocator.reset(
          new functor::CudnnBatchNormAllocatorInTemp<uint8>(context));

      // the cudnn kernel outputs inverse variance in forward and reuse it in
      // backward
      if (reserve_space.dims() != 0) {
        reserve_space_data = functor::CastDeviceMemory<uint8, U>(
            const_cast<Tensor*>(&reserve_space));
        reserve_space_data_ptr = &reserve_space_data;
      }
    }
#endif  // CUDNN_VERSION >= 7402

    bool cudnn_launch_status =
        stream
            ->ThenBatchNormalizationBackward(
                y_backprop_ptr, x_ptr, scale_ptr, mean_ptr, inv_variance_ptr,
                x_desc, scale_offset_desc, static_cast<double>(epsilon),
                &x_backprop_ptr, &scale_backprop_ptr, &offset_backprop_ptr,
                reserve_space_data_ptr, workspace_allocator.get())
            .ok();

    if (!cudnn_launch_status) {
      context->SetStatus(
          errors::Internal("cuDNN launch failure : input shape (",
                           x.shape().DebugString(), ")"));
    }
    if (tensor_format == FORMAT_NHWC && compute_format == FORMAT_NCHW) {
      functor::NCHWToNHWC<GPUDevice, T, 4>()(
          context->eigen_device<GPUDevice>(),
          const_cast<const Tensor&>(x_backprop_transformed).tensor<T, 4>(),
          x_backprop->tensor<T, 4>());
    }
  }