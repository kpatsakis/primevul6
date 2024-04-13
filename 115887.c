bool OperandsBroadcastToOutputType(Type a, Type b, Type expected_output) {
  Type output_element_type =
      expected_output.cast<ShapedType>().getElementType();
  Type broadcasted_type =
      OpTrait::util::getBroadcastedType(a, b, output_element_type);
  return broadcasted_type != Type() && broadcasted_type == expected_output;
}