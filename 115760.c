Status ParseActivationMode(OpKernelConstruction* context,
                           FusedBatchNormActivationMode* activation_mode) {
  string activation_mode_str;
  TF_RETURN_IF_ERROR(context->GetAttr("activation_mode", &activation_mode_str));

  if (activation_mode_str == "Identity") {
    *activation_mode = FusedBatchNormActivationMode::kIdentity;
    return Status::OK();
  }
  if (activation_mode_str == "Relu") {
    *activation_mode = FusedBatchNormActivationMode::kRelu;
    return Status::OK();
  }
  return errors::InvalidArgument("Unsupported activation mode: ",
                                 activation_mode_str);
}