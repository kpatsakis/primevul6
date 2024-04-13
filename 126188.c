void ValidateInputs(bool is_save_op, OpKernelContext* context,
                    const Tensor& prefix, const Tensor& tensor_names,
                    const Tensor& shape_and_slices) {
  const int kFixedInputs = 3;  // Prefix, tensor names, shape_and_slices.
  const int num_tensors = static_cast<int>(tensor_names.NumElements());
  OP_REQUIRES(
      context, prefix.NumElements() == 1,
      errors::InvalidArgument("Input prefix should have a single element, got ",
                              prefix.NumElements(), " instead."));
  OP_REQUIRES(context,
              TensorShapeUtils::IsVector(tensor_names.shape()) &&
                  TensorShapeUtils::IsVector(shape_and_slices.shape()),
              errors::InvalidArgument(
                  "Input tensor_names and shape_and_slices "
                  "should be an 1-D tensors, got ",
                  tensor_names.shape().DebugString(), " and ",
                  shape_and_slices.shape().DebugString(), " instead."));
  OP_REQUIRES(context,
              tensor_names.NumElements() == shape_and_slices.NumElements(),
              errors::InvalidArgument("tensor_names and shape_and_slices "
                                      "have different number of elements: ",
                                      tensor_names.NumElements(), " vs. ",
                                      shape_and_slices.NumElements()));
  OP_REQUIRES(context,
              FastBoundsCheck(tensor_names.NumElements() + kFixedInputs,
                              std::numeric_limits<int>::max()),
              errors::InvalidArgument("Too many inputs to the op"));
  OP_REQUIRES(
      context, shape_and_slices.NumElements() == num_tensors,
      errors::InvalidArgument("Expected ", num_tensors,
                              " elements in shapes_and_slices, but got ",
                              context->input(2).NumElements()));
  if (is_save_op) {
    OP_REQUIRES(context, context->num_inputs() == num_tensors + kFixedInputs,
                errors::InvalidArgument(
                    "Got ", num_tensors, " tensor names but ",
                    context->num_inputs() - kFixedInputs, " tensors."));
    OP_REQUIRES(context, context->num_inputs() == num_tensors + kFixedInputs,
                errors::InvalidArgument(
                    "Expected a total of ", num_tensors + kFixedInputs,
                    " inputs as input #1 (which is a string "
                    "tensor of saved names) contains ",
                    num_tensors, " names, but received ", context->num_inputs(),
                    " inputs"));
  }
}