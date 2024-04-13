  void Compute(OpKernelContext* c) override {
    const Tensor& tensor = c->input(0);

    Summary s;
    Summary::Value* v = s.add_value();
    v->set_node_name(c->op_kernel().name());

    if (tensor.dtype() == DT_STRING) {
      // tensor_util.makeNdarray doesn't work for strings in tensor_content
      tensor.AsProtoField(v->mutable_tensor());
    } else {
      tensor.AsProtoTensorContent(v->mutable_tensor());
    }

    Tensor* summary_tensor = nullptr;
    OP_REQUIRES_OK(c, c->allocate_output(0, TensorShape({}), &summary_tensor));
    CHECK(SerializeToTString(s, &summary_tensor->scalar<tstring>()()));
  }