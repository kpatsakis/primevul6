  void operator()(OpKernelContext* context, const Tensor& y_backprop_input,
                  const Tensor& x_input, const Tensor& scale_input,
                  const Tensor& pop_mean_input,
                  const Tensor& pop_variance_input, U epsilon,
                  Tensor* x_backprop_output, Tensor* scale_backprop_output,
                  Tensor* offset_backprop_output) {
    typename TTypes<T, 4>::ConstTensor y_backprop(
        y_backprop_input.tensor<T, 4>());
    typename TTypes<T, 4>::ConstTensor input(x_input.tensor<T, 4>());
    typename TTypes<U>::ConstVec scale(scale_input.vec<U>());
    typename TTypes<U>::ConstVec pop_mean(pop_mean_input.vec<U>());
    typename TTypes<U>::ConstVec pop_var(pop_variance_input.vec<U>());
    typename TTypes<T, 4>::Tensor x_backprop(x_backprop_output->tensor<T, 4>());
    typename TTypes<U>::Vec scale_backprop(scale_backprop_output->vec<U>());

    const int depth = pop_mean.dimension(0);
    const int rest_size = input.size() / depth;

    const CPUDevice& d = context->eigen_device<CPUDevice>();

    // Allocate two temporary workspaces of [depth] shape.
    Tensor scratch1_vec, scratch2_vec;
    OP_REQUIRES_OK(context, context->allocate_temp(DataTypeToEnum<U>::value,
                                                   {depth}, &scratch1_vec));
    OP_REQUIRES_OK(context, context->allocate_temp(DataTypeToEnum<U>::value,
                                                   {depth}, &scratch2_vec));

    // Maybe allocate a temporary workspace of [rest_size, depth] shape.
    Tensor scratch3_tensor;
    if (std::is_same<T, U>::value) {
      OP_REQUIRES(
          context,
          scratch3_tensor.CopyFrom(*x_backprop_output, {rest_size, depth}),
          errors::Internal("Failed to copy a tensor"));
    } else {
      OP_REQUIRES_OK(context, context->allocate_temp(DataTypeToEnum<U>::value,
                                                     {rest_size, depth},
                                                     &scratch3_tensor));
    }

    typename TTypes<U>::Vec scratch1(scratch1_vec.vec<U>());
    typename TTypes<U>::Vec scratch2(scratch2_vec.vec<U>());
    typename TTypes<U, 2>::Tensor scratch3(scratch3_tensor.tensor<U, 2>());

    Eigen::DSizes<Eigen::Index, 2> rest_by_depth(rest_size, depth);
#if !defined(EIGEN_HAS_INDEX_LIST)
    Eigen::DSizes<Eigen::Index, 2> one_by_depth(1, depth);
    Eigen::array<int, 2> rest_by_one({rest_size, 1});
#else
    Eigen::IndexList<Eigen::type2index<1>, Eigen::Index> one_by_depth;
    one_by_depth.set(1, depth);
    Eigen::IndexList<Eigen::Index, Eigen::type2index<1>> rest_by_one;
    rest_by_one.set(0, rest_size);
#endif

    // Sum reduction along the 0th dimension using custom CPU functor.
    using ScalarSum = Eigen::internal::scalar_sum_op<U>;
    const functor::ReduceOuterDimensions<T, U, U, ScalarSum> redux_sum_t;
    const functor::ReduceOuterDimensions<U, U, U, ScalarSum> redux_sum_u;

    // offset_backprop  = sum(y_backprop)
    // scale_backprop = y_backprop * ((x - pop_mean) * rsqrt(pop_var + epsilon))
    // x_backprop = y_backprop * (scale * rsqrt(pop_var + epsilon))

    // NOTE: DEFAULT DEVICE comment is added to expression assignments that
    // we don't want to be executed in a thread pool.

    auto y_backprop_rest_by_depth =
        y_backprop.reshape(rest_by_depth).template cast<U>();
    auto input_rest_by_depth = input.reshape(rest_by_depth).template cast<U>();

    // offset_backprop  = sum(y_backprop)
    redux_sum_t(d, rest_by_depth, y_backprop_input, offset_backprop_output);

    // scratch1 = rsqrt(pop_var + epsilon)
    scratch1 = (pop_var + pop_var.constant(epsilon)).rsqrt();  // DEFAULT DEVICE

    // scratch2 = sum(y_backprop * (x - mean))
    scratch3.device(d) =
        y_backprop_rest_by_depth *
        (input_rest_by_depth -
         pop_mean.reshape(one_by_depth).broadcast(rest_by_one));
    redux_sum_u(d, rest_by_depth, scratch3_tensor, &scratch2_vec);

    x_backprop.reshape(rest_by_depth).device(d) =
        (y_backprop_rest_by_depth *
         ((scratch1.reshape(one_by_depth) * scale.reshape(one_by_depth))
              .broadcast(rest_by_one)))
            .template cast<T>();
    scale_backprop = scratch2 * scratch1;  // DEFAULT DEVICE
  }