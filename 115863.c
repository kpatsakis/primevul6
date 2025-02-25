DenseElementsAttr GetShape(Value output_val) {
  auto output_type = output_val.getType().cast<RankedTensorType>();
  auto shape_vector = output_type.getShape();
  std::vector<int32_t> shape;
  shape.reserve(shape_vector.size());
  for (auto shape_object : shape_vector) {
    shape.push_back(shape_object);
  }
  return mlir::DenseElementsAttr::get(
      RankedTensorType::get(
          {static_cast<int>(shape.size())},
          mlir::IntegerType::get(output_val.getContext(), 32)),
      llvm::makeArrayRef(shape));
}