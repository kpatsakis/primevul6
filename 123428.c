Status ForwardInputOrCreateNewList(OpKernelContext* c, int32 input_index,
                                   int32 output_index,
                                   const TensorList& input_list,
                                   TensorList** output_list) {
  // Attempt to forward the input tensor to the output if possible.
  std::unique_ptr<Tensor> maybe_output = c->forward_input(
      input_index, output_index, DT_VARIANT, TensorShape{},
      c->input_memory_type(input_index), AllocatorAttributes());
  Tensor* output_tensor;
  if (maybe_output != nullptr && maybe_output->dtype() == DT_VARIANT &&
      maybe_output->NumElements() == 1) {
    output_tensor = maybe_output.get();
    TensorList* tmp_out = output_tensor->scalar<Variant>()().get<TensorList>();
    if (tmp_out == nullptr) {
      return errors::InvalidArgument(
          "Expected input ", input_index, " to be a TensorList but saw ",
          output_tensor->scalar<Variant>()().TypeName());
    }
    if (tmp_out->RefCountIsOne()) {
      // Woohoo, forwarding succeeded!
      c->set_output(output_index, *output_tensor);
      *output_list = tmp_out;
      return Status::OK();
    }
  }

  // If forwarding is not possible allocate a new output tensor and copy
  // the `input_list` to it.
  AllocatorAttributes attr;
  attr.set_on_host(true);
  TF_RETURN_IF_ERROR(
      c->allocate_output(output_index, {}, &output_tensor, attr));
  output_tensor->scalar<Variant>()() = input_list.Copy();

  *output_list = output_tensor->scalar<Variant>()().get<TensorList>();
  return Status::OK();
}