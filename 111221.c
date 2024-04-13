  void Compute(OpKernelContext* context) override {
    const Tensor& x_input = context->input(0);
    const Tensor& y_input = context->input(1);
    OP_REQUIRES(
        context, x_input.shape() == y_input.shape(),
        errors::InvalidArgument("x and y must be of the same shape. ",
                                "x shape: ", x_input.shape().DebugString(),
                                ". y shape: ", y_input.shape().DebugString()));
    Tensor* z_output = nullptr;
    OP_REQUIRES_OK(context,
                   context->allocate_output(0, x_input.shape(), &z_output));
    const Device& d = context->eigen_device<Device>();
    typename TTypes<T>::ConstFlat x(x_input.flat<T>());
    typename TTypes<T>::ConstFlat y(y_input.flat<T>());
    typename TTypes<bool>::Flat z(z_output->flat<bool>());
    functor::ApproximateEqual<Device, T>()(d, x, y, tolerance_, z);
  }