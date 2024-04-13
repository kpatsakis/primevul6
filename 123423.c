  void Compute(OpKernelContext* c) override {
    const Tensor& input = c->input(1);
    OP_REQUIRES(c, element_dtype_ == input.dtype(),
                errors::InvalidArgument("Invalid data types; list elements ",
                                        DataTypeString(element_dtype_),
                                        " but tried to append ",
                                        DataTypeString(input.dtype())));

    const TensorList* l = nullptr;
    OP_REQUIRES_OK(c, GetInputList(c, 0, &l));
    OP_REQUIRES(c, l->element_shape.IsCompatibleWith(input.shape()),
                errors::InvalidArgument(
                    "Tried to append a tensor with incompatible shape to a "
                    "list. Op element shape: ",
                    input.shape().DebugString(),
                    " list shape: ", l->element_shape.DebugString()));
    OP_REQUIRES(c, element_dtype_ == l->element_dtype,
                errors::InvalidArgument("Invalid data types; op elements ",
                                        DataTypeString(element_dtype_),
                                        " but list elements ",
                                        DataTypeString(l->element_dtype)));

    if (l->max_num_elements != -1) {
      OP_REQUIRES(
          c, l->tensors().size() < l->max_num_elements,
          errors::InvalidArgument("Tried to push item into a full list",
                                  " list size: ", l->tensors().size(),
                                  " max_num_elements: ", l->max_num_elements));
    }

    TensorList* output_list = nullptr;
    OP_REQUIRES_OK(c, ForwardInputOrCreateNewList(c, 0, 0, *l, &output_list));
    output_list->tensors().push_back(input);
  }