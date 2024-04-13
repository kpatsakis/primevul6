void ciEnv::record_failure(const char* reason) {
  if (_failure_reason == NULL) {
    // Record the first failure reason.
    _failure_reason = reason;
  }
}