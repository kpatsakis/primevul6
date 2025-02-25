static Type GetShapeStrippedType(TypeAttr type_attr) {
  auto type = type_attr.getValue();
  auto shaped_type = type.dyn_cast<ShapedType>();
  if (shaped_type) {
    return shaped_type.getElementType();
  } else {
    return type;
  }
}