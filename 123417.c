  void Compute(OpKernelContext* c) override {
    const TensorList* input_list = nullptr;
    OP_REQUIRES_OK(c, GetInputList(c, 0, &input_list));
    int32 size = c->input(1).scalar<int32>()();
    OP_REQUIRES(
        c, size >= 0,
        errors::InvalidArgument(
            "TensorListSlice expects size to be non-negative. Got: ", size));

    std::unique_ptr<Tensor> maybe_result =
        c->forward_input(0, 0, DT_VARIANT, TensorShape{},
                         c->input_memory_type(0), AllocatorAttributes());
    if (maybe_result != nullptr) {
      TensorList* out = maybe_result->scalar<Variant>()().get<TensorList>();
      if (out->RefCountIsOne()) {
        // We are able to forward the input.
        out->tensors().resize(size, Tensor(DT_INVALID));
        c->set_output(0, *maybe_result);
        return;
      }
    }

    // We were not able to forward the input.  Will have to resize from scratch.
    Tensor* result;
    AllocatorAttributes attr;
    attr.set_on_host(true);
    OP_REQUIRES_OK(c, c->allocate_output(0, TensorShape{}, &result, attr));
    TensorList output_list;
    output_list.element_shape = input_list->element_shape;
    output_list.element_dtype = input_list->element_dtype;
    output_list.max_num_elements = input_list->max_num_elements;
    if (size > input_list->tensors().size()) {
      output_list.tensors().insert(output_list.tensors().begin(),
                                   input_list->tensors().begin(),
                                   input_list->tensors().end());
      // Add DT_INVALID tensors to the end of the list if the requested size
      // is larger than the list length.
      output_list.tensors().resize(size, Tensor(DT_INVALID));
    } else {
      output_list.tensors().insert(output_list.tensors().begin(),
                                   input_list->tensors().begin(),
                                   input_list->tensors().begin() + size);
    }
    result->scalar<Variant>()() = std::move(output_list);
  }