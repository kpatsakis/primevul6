  void operator()(OpKernelContext* context, const Tensor& y_backprop_input,
                  const Tensor& x_input, const Tensor& scale_input,
                  const Tensor& mean_input, const Tensor& variance_input,
                  U epsilon, Tensor* x_backprop_output,
                  Tensor* scale_backprop_output, Tensor* offset_backprop_output,
                  bool use_reserved_space, TensorFormat tensor_format) {
    Tensor transformed_y_backprop_input;
    Tensor transformed_x_input;
    Tensor transformed_x_backprop_output;
    if (tensor_format == FORMAT_NCHW) {
      const int64 in_batch = GetTensorDim(x_input, tensor_format, 'N');
      const int64 in_rows = GetTensorDim(x_input, tensor_format, 'H');
      const int64 in_cols = GetTensorDim(x_input, tensor_format, 'W');
      const int64 in_depths = GetTensorDim(x_input, tensor_format, 'C');
      OP_REQUIRES_OK(context, context->allocate_temp(
                                  DataTypeToEnum<T>::value,
                                  ShapeFromFormat(FORMAT_NHWC, in_batch,
                                                  in_rows, in_cols, in_depths),
                                  &transformed_y_backprop_input));
      OP_REQUIRES_OK(context, context->allocate_temp(
                                  DataTypeToEnum<T>::value,
                                  ShapeFromFormat(FORMAT_NHWC, in_batch,
                                                  in_rows, in_cols, in_depths),
                                  &transformed_x_input));
      OP_REQUIRES_OK(context, context->allocate_temp(
                                  DataTypeToEnum<T>::value,
                                  ShapeFromFormat(FORMAT_NHWC, in_batch,
                                                  in_rows, in_cols, in_depths),
                                  &transformed_x_backprop_output));
      // Perform NCHW to NHWC
      std::vector<int32> perm = {0, 2, 3, 1};
      OP_REQUIRES_OK(
          context, ::tensorflow::DoTranspose(context->eigen_device<CPUDevice>(),
                                             y_backprop_input, perm,
                                             &transformed_y_backprop_input));
      OP_REQUIRES_OK(context, ::tensorflow::DoTranspose(
                                  context->eigen_device<CPUDevice>(), x_input,
                                  perm, &transformed_x_input));
    } else {
      transformed_y_backprop_input = y_backprop_input;
      transformed_x_input = x_input;
      transformed_x_backprop_output = *x_backprop_output;
    }
    typename TTypes<T, 4>::Tensor y_backprop(
        transformed_y_backprop_input.tensor<T, 4>());
    typename TTypes<T, 4>::Tensor x(transformed_x_input.tensor<T, 4>());
    typename TTypes<U>::ConstVec scale(scale_input.vec<U>());
    typename TTypes<U>::ConstVec mean(mean_input.vec<U>());
    typename TTypes<U>::ConstVec variance(variance_input.vec<U>());
    typename TTypes<T, 4>::Tensor x_backprop(
        transformed_x_backprop_output.tensor<T, 4>());
    typename TTypes<U>::Vec offset_backprop(offset_backprop_output->vec<U>());

    // Note: the following formulas are used to compute the gradients for
    // back propagation.
    // x_backprop = scale * rsqrt(variance + epsilon) *
    //              [y_backprop - mean(y_backprop) - (x - mean(x)) *
    //              mean(y_backprop * (x - mean(x))) / (variance + epsilon)]
    // scale_backprop = sum(y_backprop *
    //                  (x - mean(x)) * rsqrt(variance + epsilon))
    // offset_backprop = sum(y_backprop)

    const CPUDevice& d = context->eigen_device<CPUDevice>();
    const int depth = x.dimension(3);
    const int size = x.size();
    const int rest_size = size / depth;
    Eigen::DSizes<Eigen::Index, 2> rest_by_depth(rest_size, depth);

#if !defined(EIGEN_HAS_INDEX_LIST)
    Eigen::DSizes<Eigen::Index, 2> one_by_depth(1, depth);
    Eigen::array<int, 2> bcast_spec({rest_size, 1});
#else
    Eigen::IndexList<Eigen::type2index<1>, Eigen::Index> one_by_depth;
    one_by_depth.set(1, depth);
    Eigen::IndexList<Eigen::Index, Eigen::type2index<1>> bcast_spec;
    bcast_spec.set(0, rest_size);
#endif

    auto x_rest_by_depth = x.reshape(rest_by_depth).template cast<U>();
    U rest_size_inv = static_cast<U>(1.0f / static_cast<U>(rest_size));

    // Eigen is notoriously bad at reducing outer dimension, so we materialize
    // all temporary tensors that require reduction, and then use Eigen redux
    // functor, that is optimized for this particular task.
    //
    // All reductions are of this type: [rest_size, depth] -> [depth].
    using ScalarSum = Eigen::internal::scalar_sum_op<U>;
    const functor::ReduceOuterDimensions<T, U, U, ScalarSum> redux_sum_t;
    const functor::ReduceOuterDimensions<U, U, U, ScalarSum> redux_sum_u;

    auto scratch_dtype = DataTypeToEnum<U>::value;

    // Allocate a temporary workspace of [depth] shape.
    Tensor scratch_one_by_depth;
    OP_REQUIRES_OK(context, context->allocate_temp(scratch_dtype, {depth},
                                                   &scratch_one_by_depth));

    // Maybe allocate a temporary workspace of [rest_size, depth] shape.
    Tensor scratch_rest_by_depth;
    if (std::is_same<T, U>::value) {
      OP_REQUIRES(context,
                  scratch_rest_by_depth.CopyFrom(transformed_x_backprop_output,
                                                 {rest_size, depth}),
                  errors::Internal("Failed to copy a tensor"));
    } else {
      OP_REQUIRES_OK(context,
                     context->allocate_temp(scratch_dtype, {rest_size, depth},
                                            &scratch_rest_by_depth));
    }

    typename TTypes<U, 2>::Tensor scratch_tensor(
        scratch_rest_by_depth.tensor<U, 2>());
    typename TTypes<U>::Vec scratch_vector(scratch_one_by_depth.vec<U>());

    auto x_mean_rest_by_depth =
        mean.reshape(one_by_depth).broadcast(bcast_spec);
    auto x_centered = (x_rest_by_depth - x_mean_rest_by_depth);
    auto coef0_one_by_depth =
        (variance.reshape(one_by_depth) + epsilon).rsqrt();
    auto coef0_rest_by_depth = coef0_one_by_depth.broadcast(bcast_spec);
    auto x_scaled = x_centered * coef0_rest_by_depth;

    auto y_backprop_rest_by_depth =
        y_backprop.reshape(rest_by_depth).template cast<U>();

    // Compute `scale_backprop_output`:
    //   scale_backprop =
    //     (y_backprop_rest_by_depth * x_scaled).sum(reduce_dims)
    scratch_tensor.device(d) = y_backprop_rest_by_depth * x_scaled;
    redux_sum_u(d, rest_by_depth, scratch_rest_by_depth, scale_backprop_output);

    // Compute 'offset_backprop_output':
    //   offset_backprop =
    //     y_backprop_rest_by_depth.sum(reduce_dims)
    redux_sum_t(d, rest_by_depth, transformed_y_backprop_input,
                offset_backprop_output);
    auto y_backprop_sum = offset_backprop;

    auto y_backprop_sum_one_by_depth = y_backprop_sum.reshape(one_by_depth);
    auto y_backprop_mean_one_by_depth =
        y_backprop_sum_one_by_depth * rest_size_inv;
    auto y_backprop_mean_rest_by_depth =
        y_backprop_mean_one_by_depth.broadcast(bcast_spec);
    auto y_backprop_centered =
        y_backprop_rest_by_depth - y_backprop_mean_rest_by_depth;

    // Compute expression:
    //   y_backprop_centered_mean =
    //     (y_backprop_rest_by_depth * x_centered).mean(reduce_dims)
    scratch_tensor.device(d) = y_backprop_rest_by_depth * x_centered;
    redux_sum_u(d, rest_by_depth, scratch_rest_by_depth, &scratch_one_by_depth);
    auto y_backprop_centered_mean =
        scratch_vector.reshape(one_by_depth) / static_cast<U>(rest_size);

    auto coef1 = (scale.reshape(one_by_depth) * coef0_one_by_depth)
                     .broadcast(bcast_spec);
    auto coef2 = (coef0_one_by_depth.square() * y_backprop_centered_mean)
                     .broadcast(bcast_spec);

    x_backprop.reshape(rest_by_depth).device(d) =
        (coef1 * (y_backprop_centered - x_centered * coef2)).template cast<T>();

    if (tensor_format == FORMAT_NCHW) {
      // Perform NHWC to NCHW
      std::vector<int32> perm = {0, 3, 1, 2};
      OP_REQUIRES_OK(
          context, ::tensorflow::DoTranspose(context->eigen_device<CPUDevice>(),
                                             transformed_x_backprop_output,
                                             perm, x_backprop_output));
    }
  }