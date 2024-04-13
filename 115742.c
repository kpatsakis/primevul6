string ToString(FusedBatchNormActivationMode activation_mode) {
  switch (activation_mode) {
    case FusedBatchNormActivationMode::kIdentity:
      return "Identity";
    case FusedBatchNormActivationMode::kRelu:
      return "Relu";
  }
}