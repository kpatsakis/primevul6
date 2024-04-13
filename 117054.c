Status RaggedComponentsFromVariant(
    const Tensor& encoded_variant, int ragged_rank, DataType value_dtype,
    DataType split_dtype, std::vector<RaggedTensorVariant>* decoded_ragged) {
  const auto& flat_variants = encoded_variant.flat<Variant>();
  decoded_ragged->reserve(flat_variants.size());

  for (int i = 0; i < flat_variants.size(); i++) {
    const auto& flat_variant = flat_variants(i);
    const RaggedTensorVariant* decoded =
        flat_variant.get<RaggedTensorVariant>();
    if (decoded == nullptr) {
      return errors::InvalidArgument(
          "Input Variant element at index ", i,
          " doesn't hold a RaggedTensorVariant: ", flat_variant.DebugString());
    }
    decoded_ragged->push_back(*decoded);
    decoded = &decoded_ragged->back();
    // Check ragged rank & types
    if (decoded->ragged_rank() != ragged_rank) {
      return errors::InvalidArgument(
          "Encoded input RaggedTensorVariant has ragged_rank=",
          decoded->ragged_rank(), ".  Expected ragged_rank=", ragged_rank, ".");
    }
    if (decoded->values().dtype() != value_dtype) {
      return errors::InvalidArgument(
          "Expected values Tensor dtype: ", DataTypeString(value_dtype),
          ", found: ", DataTypeString(decoded->values().dtype()));
    }
    if (decoded->values().dims() < 1) {
      return errors::InvalidArgument(
          "Ragged values must have rank >= 1; encoded scalar element at index ",
          i, " has values Tensor: ", decoded->values().DebugString());
    }
    for (const auto& splits : decoded->nested_splits()) {
      if (splits.dtype() != split_dtype) {
        return errors::InvalidArgument(
            "Expected row_splits Tensor dtype: ", DataTypeString(split_dtype),
            ", found: ", DataTypeString(splits.dtype()));
      }
      if (splits.dims() != 1) {
        return errors::InvalidArgument(
            "Ragged splits must have rank 1; encoded scalar element at index ",
            i, " has splits Tensor ", splits.DebugString());
      }
    }
  }
  return Status::OK();
}