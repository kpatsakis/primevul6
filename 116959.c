TF_Status* ReleaseThreadLocalStatus() {
  if (thread_local_tf_status == nullptr) {
    return nullptr;
  }
  return thread_local_tf_status.release();
}