  void Compute(OpKernelContext* c) override {
    const TensorList* l = nullptr;
    OP_REQUIRES_OK(c, GetInputList(c, 0, &l));
    Tensor* result;
    if (l->element_shape.unknown_rank()) {
      OP_REQUIRES_OK(c, c->allocate_output(0, TensorShape({}), &result));
      if (result->dtype() == DT_INT32) {
        result->scalar<int32>()() = -1;
      } else {
        result->scalar<int64>()() = -1;
      }
    } else {
      OP_REQUIRES_OK(c, c->allocate_output(
                            0, TensorShape{l->element_shape.dims()}, &result));
      for (int i = 0; i < l->element_shape.dims(); ++i) {
        if (result->dtype() == DT_INT32) {
          result->flat<int32>()(i) = l->element_shape.dim_size(i);
        } else {
          result->flat<int64>()(i) = l->element_shape.dim_size(i);
        }
      }
    }
  }