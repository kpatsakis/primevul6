FILE *LightProcess::popen(const char *cmd, const char *type,
                          const char *cwd /* = NULL */) {
  if (!Available()) {
    // fallback to normal popen
    Logger::Verbose("Light-weight fork not available; "
                    "use the heavy one instead.");
  } else {
    FILE *f = LightPopenImpl(cmd, type, cwd);
    if (f) {
      return f;
    }
    Logger::Verbose("Light-weight fork failed; use the heavy one instead.");
  }
  return HeavyPopenImpl(cmd, type, cwd);
}