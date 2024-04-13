inline bool HasAccumulatorOrTape() {
  return HasGradientTape() || HasAccumulator();
}