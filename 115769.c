se::dnn::ActivationMode AsDnnActivationMode(
    const FusedBatchNormActivationMode activation_mode) {
  switch (activation_mode) {
    case FusedBatchNormActivationMode::kIdentity:
      return se::dnn::ActivationMode::kNone;
    case FusedBatchNormActivationMode::kRelu:
      return se::dnn::ActivationMode::kRelu;
  }
}