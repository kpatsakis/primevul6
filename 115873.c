bool IsTailOfShape(Type type1, Type type2) {
  auto tail_type = type1.dyn_cast<ShapedType>();
  auto full_type = type2.dyn_cast<ShapedType>();
  if (!tail_type || !full_type || !tail_type.hasRank() ||
      !full_type.hasRank() || tail_type.getRank() > full_type.getRank())
    return false;
  auto i1 = tail_type.getShape().rbegin(), e1 = tail_type.getShape().rend();
  auto i2 = full_type.getShape().rbegin();
  return std::equal(i1, e1, i2);
}