ElementsAttr ExpandTo4DForDepthwiseConv(Attribute a) {
  return ExpandTo4DForConvImpl(a, true);
}