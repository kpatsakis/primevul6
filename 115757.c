  virtual void ComputeWithReservedSpace(OpKernelContext* context,
                                        bool use_reserved_space) {
    Tensor y_backprop = context->input(0);
    Tensor x = context->input(1);
    const Tensor& scale = context->input(2);
    // When is_training=True, batch mean and variance/inverted variance are
    // saved in the forward pass to be reused here. When is_training=False,
    // population mean and variance need to be forwarded here to compute the
    // gradients.
    const Tensor& saved_mean_or_pop_mean = context->input(3);
    // The Eigen implementation saves variance in the forward pass, while cuDNN
    // saves inverted variance.
    const Tensor& saved_maybe_inv_var_or_pop_var = context->input(4);

    OP_REQUIRES(context, y_backprop.dims() == 4 || y_backprop.dims() == 5,
                errors::InvalidArgument("input must be 4 or 5-dimensional",
                                        y_backprop.shape().DebugString()));
    OP_REQUIRES(context, x.dims() == 4 || x.dims() == 5,
                errors::InvalidArgument("input must be 4 or 5-dimensional",
                                        x.shape().DebugString()));
    OP_REQUIRES(context, scale.dims() == 1,
                errors::InvalidArgument("scale must be 1-dimensional",
                                        scale.shape().DebugString()));
    OP_REQUIRES(
        context, saved_mean_or_pop_mean.dims() == 1,
        errors::InvalidArgument("saved mean must be 1-dimensional",
                                saved_mean_or_pop_mean.shape().DebugString()));
    OP_REQUIRES(context, saved_maybe_inv_var_or_pop_var.dims() == 1,
                errors::InvalidArgument(
                    "saved variance must be 1-dimensional",
                    saved_maybe_inv_var_or_pop_var.shape().DebugString()));
    bool use_reshape = (x.dims() == 5);
    auto x_shape = x.shape();
    TensorShape dest_shape;
    if (use_reshape) {
      const int64 in_batch = GetTensorDim(x, tensor_format_, 'N');
      int64 in_planes = GetTensorDim(x, tensor_format_, '0');
      int64 in_rows = GetTensorDim(x, tensor_format_, '1');
      int64 in_cols = GetTensorDim(x, tensor_format_, '2');
      const int64 in_depth = GetTensorDim(x, tensor_format_, 'C');
      dest_shape = ShapeFromFormat(tensor_format_, in_batch,
                                   {{in_planes, in_rows * in_cols}}, in_depth);
      OP_REQUIRES(context, x.CopyFrom(x, dest_shape),
                  errors::InvalidArgument("Error during tensor copy."));
      OP_REQUIRES(context, y_backprop.CopyFrom(y_backprop, dest_shape),
                  errors::InvalidArgument("Error during tensor copy."));
    }

    Tensor* x_backprop = nullptr;
    auto alloc_shape = use_reshape ? dest_shape : x_shape;
    OP_REQUIRES_OK(context,
                   context->allocate_output(0, alloc_shape, &x_backprop));

    const TensorShape& scale_offset_shape = scale.shape();
    Tensor* scale_backprop = nullptr;
    OP_REQUIRES_OK(context, context->allocate_output(1, scale_offset_shape,
                                                     &scale_backprop));
    Tensor* offset_backprop = nullptr;
    OP_REQUIRES_OK(context, context->allocate_output(2, scale_offset_shape,
                                                     &offset_backprop));
    // Two placeholders for estimated_mean and estimated_variance, which are
    // used for inference and thus not needed here for gradient computation.
    // They are filled with zeros so as to avoid NaN outputs.
    Tensor* placeholder_1 = nullptr;
    OP_REQUIRES_OK(
        context, context->allocate_output(3, TensorShape({0}), &placeholder_1));
    Tensor* placeholder_2 = nullptr;
    OP_REQUIRES_OK(
        context, context->allocate_output(4, TensorShape({0}), &placeholder_2));

    // If input is empty, set gradients w.r.t scale/offset to zero.
    if (x.shape().num_elements() == 0) {
      functor::SetZeroFunctor<Device, U> f;
      f(context->eigen_device<Device>(), scale_backprop->flat<U>());
      f(context->eigen_device<Device>(), offset_backprop->flat<U>());
      return;
    }

    if (is_training_) {
      functor::FusedBatchNormGrad<Device, T, U>()(
          context, y_backprop, x, scale, saved_mean_or_pop_mean,
          saved_maybe_inv_var_or_pop_var, epsilon_, x_backprop, scale_backprop,
          offset_backprop, use_reserved_space, tensor_format_);
    } else {
      // Necessary layout conversion is currently done in python.
      OP_REQUIRES(context, tensor_format_ == FORMAT_NHWC,
                  errors::InvalidArgument(
                      "The implementation of "
                      "FusedBatchNormGrad with is_training=False only support "
                      "NHWC tensor format for now."));
      functor::FusedBatchNormFreezeGrad<Device, T, U>()(
          context, y_backprop, x, scale, saved_mean_or_pop_mean,
          saved_maybe_inv_var_or_pop_var, epsilon_, x_backprop, scale_backprop,
          offset_backprop);
    }
    if (use_reshape) {
      OP_REQUIRES(context, x_backprop->CopyFrom(*x_backprop, x_shape),
                  errors::InvalidArgument("Error during tensor copy."));
    }
  }