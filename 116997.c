void ReturnStatus(TF_Status* status) {
  TF_SetStatus(status, TF_OK, "");
  thread_local_tf_status.reset(status);
}