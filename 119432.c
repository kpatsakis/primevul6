  bool CreateUpdatedMultiplesProto(const NodeDef* node, TensorProto* proto) {
    Tensor multiples;
    if (!GetTensorFromConstNode(node->name(), &multiples)) {
      return false;
    }

    // Dimensions should be [X, Y, N, 1, M, 1]
    if (multiples.dtype() != DT_INT32 || multiples.NumElements() != 6) {
      return false;
    }

    const auto& multiples_values = multiples.flat<int32>();
    if (multiples_values(3) != 1 || multiples_values(5) != 1) {
      return false;
    }

    // Convert to [X, Y, N, M]
    Tensor new_multiples(DT_INT32, {4});
    new_multiples.flat<int32>()(0) = multiples_values(0);
    new_multiples.flat<int32>()(1) = multiples_values(1);
    new_multiples.flat<int32>()(2) = multiples_values(2);
    new_multiples.flat<int32>()(3) = multiples_values(4);

    new_multiples.AsProtoTensorContent(proto);
    return true;
  }