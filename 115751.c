  void operator()(OpKernelContext* context, const Tensor& x_input,
                  const Tensor& scale_input, const Tensor& offset_input,
                  const Tensor& running_mean_input,
                  const Tensor& running_variance_input,
                  const Tensor* side_input, U epsilon, U exponential_avg_factor,
                  FusedBatchNormActivationMode activation_mode,
                  Tensor* y_output, Tensor* running_mean_output,
                  Tensor* running_var_output, Tensor* saved_batch_mean_output,
                  Tensor* saved_batch_var_output, TensorFormat tensor_format,
                  bool use_reserved_space) {
    OP_REQUIRES(context, side_input == nullptr,
                errors::Internal(
                    "The CPU implementation of FusedBatchNorm does not support "
                    "side input."));
    OP_REQUIRES(context,
                activation_mode == FusedBatchNormActivationMode::kIdentity,
                errors::Internal("The CPU implementation of FusedBatchNorm "
                                 "does not support activations."));

    if (use_reserved_space) {
      Tensor* dummy_reserve_space = nullptr;
      OP_REQUIRES_OK(context,
                     context->allocate_output(5, {}, &dummy_reserve_space));
      // Initialize the memory, to avoid sanitizer alerts.
      dummy_reserve_space->flat<U>()(0) = U();
    }
    Tensor transformed_x;
    Tensor transformed_y;
    if (tensor_format == FORMAT_NCHW) {
      const int64 in_batch = GetTensorDim(x_input, tensor_format, 'N');
      const int64 in_rows = GetTensorDim(x_input, tensor_format, 'H');
      const int64 in_cols = GetTensorDim(x_input, tensor_format, 'W');
      const int64 in_depths = GetTensorDim(x_input, tensor_format, 'C');
      OP_REQUIRES_OK(context, context->allocate_temp(
                                  DataTypeToEnum<T>::value,
                                  ShapeFromFormat(FORMAT_NHWC, in_batch,
                                                  in_rows, in_cols, in_depths),
                                  &transformed_x));
      OP_REQUIRES_OK(context, context->allocate_temp(
                                  DataTypeToEnum<T>::value,
                                  ShapeFromFormat(FORMAT_NHWC, in_batch,
                                                  in_rows, in_cols, in_depths),
                                  &transformed_y));
      // Perform NCHW to NHWC
      std::vector<int32> perm = {0, 2, 3, 1};
      OP_REQUIRES_OK(
          context, ::tensorflow::DoTranspose(context->eigen_device<CPUDevice>(),
                                             x_input, perm, &transformed_x));
    } else {
      transformed_x = x_input;
      transformed_y = *y_output;
    }
    typename TTypes<T, 4>::Tensor x(transformed_x.tensor<T, 4>());
    typename TTypes<U>::ConstVec scale(scale_input.vec<U>());
    typename TTypes<U>::ConstVec offset(offset_input.vec<U>());
    typename TTypes<U>::ConstVec old_mean(running_mean_input.vec<U>());
    typename TTypes<U>::ConstVec old_variance(running_variance_input.vec<U>());
    typename TTypes<T, 4>::Tensor y(transformed_y.tensor<T, 4>());
    typename TTypes<U>::Vec new_mean(running_mean_output->vec<U>());
    typename TTypes<U>::Vec new_variance(running_var_output->vec<U>());
    typename TTypes<U>::Vec saved_batch_mean(saved_batch_mean_output->vec<U>());
    typename TTypes<U>::Vec saved_batch_var(saved_batch_var_output->vec<U>());

    const CPUDevice& d = context->eigen_device<CPUDevice>();

    const int depth = x.dimension(3);
    const int size = x.size();
    const int rest_size = size / depth;
    Eigen::DSizes<Eigen::Index, 2> rest_by_depth(rest_size, depth);

#if !defined(EIGEN_HAS_INDEX_LIST)
    Eigen::DSizes<Eigen::Index, 2> one_by_depth(1, depth);
    Eigen::array<int, 1> reduce_dims({0});
    Eigen::array<int, 2> bcast_spec({rest_size, 1});
#else
    Eigen::IndexList<Eigen::type2index<1>, Eigen::Index> one_by_depth;
    one_by_depth.set(1, depth);
    Eigen::IndexList<Eigen::type2index<0>> reduce_dims;
    Eigen::IndexList<Eigen::Index, Eigen::type2index<1>> bcast_spec;
    bcast_spec.set(0, rest_size);
#endif

    auto x_rest_by_depth = x.reshape(rest_by_depth).template cast<U>();
    const int rest_size_minus_one = (rest_size > 1) ? (rest_size - 1) : 1;
    U rest_size_inv = static_cast<U>(1.0f / static_cast<U>(rest_size));
    // This adjustment is for Bessel's correction
    U rest_size_adjust =
        static_cast<U>(rest_size) / static_cast<U>(rest_size_minus_one);

    Eigen::Tensor<U, 1, Eigen::RowMajor> batch_mean(depth);
    Eigen::Tensor<U, 1, Eigen::RowMajor> batch_variance(depth);

    batch_mean.device(d) = (x_rest_by_depth.sum(reduce_dims) * rest_size_inv);
    auto x_centered = x_rest_by_depth -
                      batch_mean.reshape(one_by_depth).broadcast(bcast_spec);

    batch_variance.device(d) =
        x_centered.square().sum(reduce_dims) * rest_size_inv;
    auto scaling_factor = ((batch_variance + epsilon).rsqrt() * scale)
                              .eval()
                              .reshape(one_by_depth)
                              .broadcast(bcast_spec);
    auto x_scaled = x_centered * scaling_factor;
    auto x_shifted =
        (x_scaled + offset.reshape(one_by_depth).broadcast(bcast_spec))
            .template cast<T>();

    y.reshape(rest_by_depth).device(d) = x_shifted;
    if (exponential_avg_factor == U(1.0)) {
      saved_batch_var.device(d) = batch_variance;
      saved_batch_mean.device(d) = batch_mean;
      new_variance.device(d) = batch_variance * rest_size_adjust;
      new_mean.device(d) = batch_mean;
    } else {
      U one_minus_factor = U(1) - exponential_avg_factor;
      saved_batch_var.device(d) = batch_variance;
      saved_batch_mean.device(d) = batch_mean;
      new_variance.device(d) =
          one_minus_factor * old_variance +
          (exponential_avg_factor * rest_size_adjust) * batch_variance;
      new_mean.device(d) =
          one_minus_factor * old_mean + exponential_avg_factor * batch_mean;
    }

    if (tensor_format == FORMAT_NCHW) {
      // Perform NHWC to NCHW
      const std::vector<int32> perm = {0, 3, 1, 2};
      const Status s = ::tensorflow::DoTranspose(
          context->eigen_device<CPUDevice>(), transformed_y, perm, y_output);
      if (!s.ok()) {
        context->SetStatus(errors::InvalidArgument("Transpose failed: ", s));
      }
    }
  }