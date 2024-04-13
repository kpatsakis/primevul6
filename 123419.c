  void Compute(OpKernelContext* c) override {
    const TensorList* l = nullptr;
    OP_REQUIRES_OK(c, GetInputList(c, 0, &l));
    OP_REQUIRES(c, element_dtype_ == l->element_dtype,
                errors::InvalidArgument("Invalid data types; op elements ",
                                        DataTypeString(element_dtype_),
                                        " but list elements ",
                                        DataTypeString(l->element_dtype)));
    int32 index = c->input(1).scalar<int32>()();
    OP_REQUIRES(c, index < l->tensors().size(),
                errors::InvalidArgument("Trying to modify element ", index,
                                        " in a list with ", l->tensors().size(),
                                        " elements."));
    const Tensor& value = c->input(2);
    OP_REQUIRES(c, l->element_shape.IsCompatibleWith(value.shape()),
                errors::InvalidArgument(
                    "Tried to set a tensor with incompatible shape at a "
                    "list index. Item element shape: ",
                    value.shape().DebugString(),
                    " list shape: ", l->element_shape.DebugString()));
    TensorList* output_list = nullptr;
    OP_REQUIRES_OK(c, ForwardInputOrCreateNewList(c, 0, 0, *l, &output_list));
    output_list->tensors()[index] = value;
  }