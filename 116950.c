TF_Status* GetStatus() {
  TF_Status* maybe_status = ReleaseThreadLocalStatus();
  if (maybe_status) {
    TF_SetStatus(maybe_status, TF_OK, "");
    return maybe_status;
  } else {
    return TF_NewStatus();
  }
}