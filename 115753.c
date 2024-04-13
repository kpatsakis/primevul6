  void operator()(OpKernelContext* context, const Tensor& x,
                  const Tensor& scale, const Tensor& offset,
                  const Tensor& estimated_mean,
                  const Tensor& estimated_variance, const Tensor* side_input,
                  U epsilon, U exponential_avg_factor,
                  FusedBatchNormActivationMode activation_mode, Tensor* y,
                  Tensor* batch_mean, Tensor* batch_var, Tensor* saved_mean,
                  Tensor* saved_inv_var, TensorFormat tensor_format,
                  bool use_reserved_space) {
    auto* stream = context->op_device_context()->stream();
    OP_REQUIRES(context, stream, errors::Internal("No GPU stream available"));

    const int64 batch_size = GetTensorDim(x, tensor_format, 'N');
    const int64 channels = GetTensorDim(x, tensor_format, 'C');
    const int64 height = GetTensorDim(x, tensor_format, 'H');
    const int64 width = GetTensorDim(x, tensor_format, 'W');

    // If use_reserved_space we have reserve_space_3 output (only in
    // FusedBatchNormV3 op).

#if GOOGLE_CUDA
    // Check if cuDNN batch normalization has a fast NHWC implementation:
    //   (1) In inference mode it's always fast.
    //   (2) Tensorflow enabled batchnorm spatial persistence, we are called
    //   from
    //       FusedBatchNormV3, i.e. use_reserved_space is true.
    const bool fast_nhwc_batch_norm =
        !is_training ||
        (BatchnormSpatialPersistentEnabled() &&
         DataTypeToEnum<T>::value == DT_HALF && use_reserved_space);
#else
    // fast NHWC implementation is a CUDA only feature
    const bool fast_nhwc_batch_norm = false;
#endif

    // If input tensor is in NHWC format, and we have a fast cuDNN
    // implementation, there is no need to do data format conversion.
    TensorFormat compute_format =
        fast_nhwc_batch_norm && tensor_format == FORMAT_NHWC ? FORMAT_NHWC
                                                             : FORMAT_NCHW;

    VLOG(2) << "FusedBatchNorm:"
            << " batch_size: " << batch_size << " channels: " << channels
            << " height: " << height << " width:" << width
            << " x shape: " << x.shape().DebugString()
            << " scale shape: " << scale.shape().DebugString()
            << " offset shape: " << offset.shape().DebugString()
            << " activation mode: " << ToString(activation_mode)
            << " tensor format: " << ToString(tensor_format)
            << " compute format: " << ToString(compute_format);

    auto maybe_make_dummy_output = [context, use_reserved_space]() -> Status {
      if (use_reserved_space) {
        Tensor* dummy_reserve_space = nullptr;
        return context->allocate_output(5, {}, &dummy_reserve_space);
      }
      return Status::OK();
    };

    // If input is empty, return NaN mean/variance
    if (x.shape().num_elements() == 0) {
      OP_REQUIRES_OK(context, maybe_make_dummy_output());
      functor::SetNanFunctor<U> f;
      f(context->eigen_device<GPUDevice>(), batch_mean->flat<U>());
      f(context->eigen_device<GPUDevice>(), batch_var->flat<U>());
      return;
    }

    // In inference mode we use custom CUDA kernel, because cuDNN does not
    // support side input and activations for inference.
    const bool has_side_input = side_input != nullptr;
    const bool has_activation =
        activation_mode != FusedBatchNormActivationMode::kIdentity;

    if (!is_training && (has_side_input || has_activation)) {
      OP_REQUIRES_OK(context, maybe_make_dummy_output());
      FusedBatchNormInferenceFunctor<GPUDevice, T, U> inference_functor;

      if (has_side_input) {
        inference_functor(context, tensor_format, x.tensor<T, 4>(),
                          scale.vec<U>(), offset.vec<U>(),
                          estimated_mean.vec<U>(), estimated_variance.vec<U>(),
                          side_input->tensor<T, 4>(), epsilon, activation_mode,
                          y->tensor<T, 4>());
      } else {
        typename TTypes<T, 4>::ConstTensor empty_tensor(nullptr, 0, 0, 0, 0);
        inference_functor(context, tensor_format, x.tensor<T, 4>(),
                          scale.vec<U>(), offset.vec<U>(),
                          estimated_mean.vec<U>(), estimated_variance.vec<U>(),
                          empty_tensor, epsilon, activation_mode,
                          y->tensor<T, 4>());
      }
      return;
    }

    Tensor x_maybe_transformed = x;
    Tensor x_transformed;
    Tensor y_transformed;
    se::DeviceMemory<T> y_ptr;

    if (tensor_format == compute_format) {
      y_ptr = StreamExecutorUtil::AsDeviceMemory<T>(*y);
    } else if (tensor_format == FORMAT_NHWC && compute_format == FORMAT_NCHW) {
      OP_REQUIRES_OK(context, context->allocate_temp(
                                  DataTypeToEnum<T>::value,
                                  ShapeFromFormat(compute_format, batch_size,
                                                  height, width, channels),
                                  &x_transformed));
      functor::NHWCToNCHW<GPUDevice, T, 4>()(
          context->eigen_device<GPUDevice>(),
          const_cast<const Tensor&>(x_maybe_transformed).tensor<T, 4>(),
          x_transformed.tensor<T, 4>());
      x_maybe_transformed = x_transformed;

      OP_REQUIRES_OK(context, context->allocate_temp(
                                  DataTypeToEnum<T>::value,
                                  ShapeFromFormat(compute_format, batch_size,
                                                  height, width, channels),
                                  &y_transformed));
      y_ptr = StreamExecutorUtil::AsDeviceMemory<T>(y_transformed);
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

    auto x_ptr = StreamExecutorUtil::AsDeviceMemory<T>(x_maybe_transformed);
    auto scale_ptr = StreamExecutorUtil::AsDeviceMemory<U>(scale);
    auto offset_ptr = StreamExecutorUtil::AsDeviceMemory<U>(offset);
    auto estimated_mean_ptr =
        StreamExecutorUtil::AsDeviceMemory<U>(estimated_mean);
    auto estimated_variance_ptr =
        StreamExecutorUtil::AsDeviceMemory<U>(estimated_variance);
    auto side_input_ptr =
        side_input != nullptr
            ? StreamExecutorUtil::AsDeviceMemory<U>(*side_input)
            : se::DeviceMemory<U>();
    auto batch_mean_ptr = StreamExecutorUtil::AsDeviceMemory<U>(*batch_mean);

    auto batch_var_ptr = StreamExecutorUtil::AsDeviceMemory<U>(*batch_var);
    auto saved_mean_ptr = StreamExecutorUtil::AsDeviceMemory<U>(*saved_mean);
    auto saved_inv_var_ptr =
        StreamExecutorUtil::AsDeviceMemory<U>(*saved_inv_var);

    std::unique_ptr<functor::CudnnBatchNormAllocatorInOutput<U>>
        reserve_space_allocator;
    std::unique_ptr<functor::CudnnBatchNormAllocatorInTemp<uint8>>
        workspace_allocator;
    if (use_reserved_space) {
      reserve_space_allocator.reset(
          new functor::CudnnBatchNormAllocatorInOutput<U>(context, 5));
      workspace_allocator.reset(
          new functor::CudnnBatchNormAllocatorInTemp<uint8>(context));
    }
    if (!batch_mean->SharesBufferWith(estimated_mean) &&
        exponential_avg_factor != 1.0f) {
      OP_REQUIRES(
          context,
          stream
              ->ThenMemcpyD2D(&batch_mean_ptr, estimated_mean_ptr,
                              estimated_mean.NumElements() * sizeof(U))
              .ok(),
          errors::Internal("MatrixTriangularSolveOp: failed to copy rhs "
                           "from device"));
    }
    if (!batch_var->SharesBufferWith(estimated_variance) &&
        exponential_avg_factor != 1.0f) {
      OP_REQUIRES(
          context,
          stream
              ->ThenMemcpyD2D(&batch_var_ptr, estimated_variance_ptr,
                              estimated_variance.NumElements() * sizeof(U))
              .ok(),
          errors::Internal("MatrixTriangularSolveOp: failed to copy rhs "
                           "from device"));
    }
    bool cudnn_launch_status =
        stream
            ->ThenBatchNormalizationForward(
                x_ptr, scale_ptr, offset_ptr, estimated_mean_ptr,
                estimated_variance_ptr, side_input_ptr, x_desc,
                scale_offset_desc, static_cast<double>(epsilon),
                static_cast<double>(exponential_avg_factor),
                AsDnnActivationMode(activation_mode), &y_ptr, &batch_mean_ptr,
                &batch_var_ptr, &saved_mean_ptr, &saved_inv_var_ptr,
                is_training, reserve_space_allocator.get(),
                workspace_allocator.get())
            .ok();

    if (!cudnn_launch_status) {
      context->SetStatus(
          errors::Internal("cuDNN launch failure : input shape (",
                           x.shape().DebugString(), ")"));
      return;
    }

    if (tensor_format == FORMAT_NHWC && compute_format == FORMAT_NCHW) {
      functor::NCHWToNHWC<GPUDevice, T, 4>()(
          context->eigen_device<GPUDevice>(),
          const_cast<const Tensor&>(y_transformed).tensor<T, 4>(),
          y->tensor<T, 4>());
    }
  }