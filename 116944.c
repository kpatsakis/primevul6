TFE_Op* GetOp(TFE_Context* ctx, const char* op_or_function_name,
              const char* raw_device_name, TF_Status* status) {
  auto op = ReleaseThreadLocalOp(ctx);
  if (!op) {
    op.reset(tensorflow::wrap(tensorflow::unwrap(ctx)->CreateOperation()));
  }
  status->status =
      tensorflow::unwrap(op.get())->Reset(op_or_function_name, raw_device_name);
  if (!status->status.ok()) {
    op.reset();
  }
  return op.release();
}