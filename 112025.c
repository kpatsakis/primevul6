  void Compute(OpKernelContext* context) override {
    const Tensor& tensor_in = context->input(0);
    PoolParameters params{context,
                          ksize_,
                          stride_,
                          padding_,
                          /*explicit_paddings=*/{},
                          FORMAT_NHWC,
                          tensor_in.shape()};
    if (!context->status().ok()) {
      return;
    }

    const float min_input = context->input(1).flat<float>()(0);
    const float max_input = context->input(2).flat<float>()(0);

    OP_REQUIRES(context, params.depth_window == 1,
                errors::Unimplemented("Non-spatial pooling is not "
                                      "yet supported. Volunteers? :)"));

    OP_REQUIRES(context, tensor_in.dims() == 4,
                errors::InvalidArgument("tensor_in must be 4-dimensional"));

    Tensor* output = nullptr;
    OP_REQUIRES_OK(context, context->allocate_output(
                                0, params.forward_output_shape(), &output));
    const int32_t highest = static_cast<int32>(Eigen::NumTraits<T>::highest());
    const int32_t lowest = static_cast<int32>(Eigen::NumTraits<T>::lowest());

    // TODO(vrv): Switch this to the Eigen::Tensor version of
    // SpatialAvgPooling once that version is running quickly.
    Tensor int32_output(DT_INT32, params.forward_output_shape());
    // Cast input to int32 tensor and call SpatialAvgPool.
    Tensor int32_input(DT_INT32, tensor_in.shape());
    int32_input.flat<int32>() = tensor_in.flat<T>().template cast<int32>();
    SpatialAvgPool<Device, int32>(context, &int32_output, int32_input, params,
                                  padding_);

    // Clamp the int32 output back into quantized space.
    output->flat<T>() = int32_output.flat<int32>()
                            .cwiseMax(lowest)
                            .cwiseMin(highest)
                            .template cast<T>();

    Tensor* output_min = nullptr;
    OP_REQUIRES_OK(context, context->allocate_output(1, {}, &output_min));
    output_min->flat<float>()(0) = min_input;
    Tensor* output_max = nullptr;
    OP_REQUIRES_OK(context, context->allocate_output(2, {}, &output_max));
    output_max->flat<float>()(0) = max_input;
  }