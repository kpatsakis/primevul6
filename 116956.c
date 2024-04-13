void ReturnOp(TFE_Context* ctx, TFE_Op* op) {
  if (op) {
    tensorflow::unwrap(op)->Clear();
    thread_local_eager_operation_map[ctx].reset(op);
  }
}