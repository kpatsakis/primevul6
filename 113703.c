FILE *LightProcess::HeavyPopenImpl(const char *cmd, const char *type,
                                   const char *cwd) {
  if (cwd && *cwd) {
    auto old_cwd = Process::GetCurrentDirectory();
    if (old_cwd != cwd) {
      Lock lock(s_mutex);
      if (chdir(cwd)) {
        Logger::Warning("Failed to chdir to %s.", cwd);
      }
      FILE *f = ::popen(cmd, type);
      if (chdir(old_cwd.c_str())) {
        // error occured changing cwd back
      }
      return f;
    }
  }
  return ::popen(cmd, type);
}