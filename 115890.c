TypeAttr RescaleQtype(Type input, Attribute factor) {
  return quant::RescaleQuantizedType(input, factor);
}