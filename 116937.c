AccumulatorSet* GetAccumulatorSet() {
  thread_local std::unique_ptr<AccumulatorSet> accumulator_set;
  thread_local ThreadLocalDestructionMarker marker;
  if (!marker.alive) {
    // This thread is being destroyed. It is unsafe to access accumulator_set.
    return nullptr;
  }
  if (accumulator_set == nullptr) {
    accumulator_set.reset(new AccumulatorSet);
  }
  return accumulator_set.get();
}