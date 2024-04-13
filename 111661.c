Status LoadSavedModel(const SessionOptions& session_options,
                      const RunOptions& run_options, const string& export_dir,
                      const std::unordered_set<string>& tags,
                      SavedModelBundle* const bundle) {
  metrics::SavedModelReadApi(kCCLoadLabel).IncrementBy(1);

  // TODO(robson): Add tests for the counters.
  const uint64 start_microseconds = Env::Default()->NowMicros();
  const Status status = LoadSavedModelInternal(session_options, run_options,
                                               export_dir, tags, bundle);
  auto log_and_count = [&](const string& status_str) {
    LOG(INFO) << "SavedModel load for tags { " << absl::StrJoin(tags, " ")
              << " }; Status: " << status_str << ": " << status << ". Took "
              << GetLatencyMicroseconds(start_microseconds) << " microseconds.";
    load_attempt_count->GetCell(export_dir, status_str)->IncrementBy(1);
  };
  if (status.ok()) {
    log_and_count(kLoadAttemptSuccess);
  } else {
    log_and_count(kLoadAttemptFail);
  }
  load_latency->GetCell(export_dir)
      ->IncrementBy(GetLatencyMicroseconds(start_microseconds));
  return status;
}