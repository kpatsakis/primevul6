bool Capability::SetDumpable() {
  if (prctl(PR_SET_DUMPABLE, 1, 0, 0, 0)) {
    Logger::Error("Unable to make process dumpable: %s",
                  folly::errnoStr(errno).c_str());
  }

  return true;
}