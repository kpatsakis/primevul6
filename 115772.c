bool BatchnormSpatialPersistentEnabled() {
#if CUDNN_VERSION >= 7402
  static bool is_enabled = [] {
    bool is_enabled = false;
    TF_CHECK_OK(tensorflow::ReadBoolFromEnvVar(
        "TF_USE_CUDNN_BATCHNORM_SPATIAL_PERSISTENT",
        /*default_val=*/false, &is_enabled));
    return is_enabled;
  }();
  return is_enabled;
#else
  return false;
#endif
}