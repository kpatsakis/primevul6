bool FastAreTensorProtosEqual(const TensorProto& lhs, const TensorProto& rhs) {
  // A small TensorProto can expand into a giant Tensor.  So we avoid
  // conversion to an actual Tensor if we can quickly rule out equality
  // by comparing the Tensor size since different sized Tensors are definitely
  // different.
  const int64 lhs_tensor_bytes = TensorByteSize(lhs);
  const int64 rhs_tensor_bytes = TensorByteSize(rhs);
  if (lhs_tensor_bytes != rhs_tensor_bytes) {
    return false;
  }

  // If the tensor is very large, we'll only compare the proto representation
  // (even though this may miss some equivalent tensors whose actual tensor
  // values are the same but which are described by different TensorProtos).
  if (lhs_tensor_bytes > kMaxAttrValueTensorByteSize) {
    return AreSerializedProtosEqual(lhs, rhs);
  }

  // If the TensorProto representation expands into a much bigger Tensor,
  // we have a fast-path that first compares the protos.
  const int64 lhs_proto_bytes = lhs.ByteSizeLong();
  const bool large_expansion =
      (lhs_proto_bytes < 512 && lhs_tensor_bytes > 4096);
  if (large_expansion && AreSerializedProtosEqual(lhs, rhs)) {
    return true;
  }

  // Fall back to the general code in AreTensorProtosEqual.
  return AreTensorProtosEqual(lhs, rhs);
}