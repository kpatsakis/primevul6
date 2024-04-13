bool CanOptimizeIdentityGatherNdOrScatterNdOp(Value params,
                                              DenseIntElementsAttr indices) {
  auto params_type = params.getType().dyn_cast<RankedTensorType>();
  auto indices_type = indices.getType().dyn_cast<RankedTensorType>();
  // Checks the shape of `params` is [n, ...], shape of `indices` is [n, 1]. 2D
  // `indices` means it gets the first row of `params`. As long as indices
  // iterate the first row of `params`, the output is identical to input.
  if (!params_type || !indices_type || indices_type.getRank() != 2 ||
      indices_type.getDimSize(0) != params_type.getDimSize(0) ||
      indices_type.getDimSize(1) != 1)
    return false;

  // Checks the value in `indices` is from 0 to n-1.
  int cur_value = 0;
  for (const auto &v : indices.getValues<APInt>()) {
    if (v.getSExtValue() != cur_value) return false;
    ++cur_value;
  }

  return true;
}