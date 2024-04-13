  void Compute(OpKernelContext* c) override {
    const TensorList* l = nullptr;
    OP_REQUIRES_OK(c, GetInputList(c, 0, &l));
    Tensor* result;
    OP_REQUIRES_OK(c, c->allocate_output(0, TensorShape{}, &result));
    result->scalar<int32>()() = l->tensors().size();
  }