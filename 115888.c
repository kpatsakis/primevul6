bool IsBroadcastableElementsAttrAndType(Type a, Type b) {
  return OpTrait::util::getBroadcastedType(a, b) != Type();
}