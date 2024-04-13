uint64 GetLatencyMicroseconds(const uint64 start_microseconds) {
  const uint64 end_microseconds = EnvTime::NowMicros();
  // Avoid clock skew.
  if (end_microseconds < start_microseconds) return 0;
  return end_microseconds - start_microseconds;
}