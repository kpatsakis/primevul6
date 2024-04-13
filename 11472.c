void ciEnv::record_method_not_compilable(const char* reason, bool all_tiers) {
  int new_compilable =
    all_tiers ? MethodCompilable_never : MethodCompilable_not_at_tier ;

  // Only note transitions to a worse state
  if (new_compilable > _compilable) {
    if (log() != NULL) {
      if (all_tiers) {
        log()->elem("method_not_compilable");
      } else {
        log()->elem("method_not_compilable_at_tier level='%d'",
                    current()->task()->comp_level());
      }
    }
    _compilable = new_compilable;

    // Reset failure reason; this one is more important.
    _failure_reason = NULL;
    record_failure(reason);
  }
}