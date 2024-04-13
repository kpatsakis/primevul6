std::unique_ptr<TFE_Op, OpDeleter> ReleaseThreadLocalOp(TFE_Context* ctx) {
  auto it = thread_local_eager_operation_map.find(ctx);
  if (it == thread_local_eager_operation_map.end()) {
    return nullptr;
  }
  return std::move(it->second);
}