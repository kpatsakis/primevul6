void LightProcess::Initialize(const std::string &prefix, int count,
                              const std::vector<int> &inherited_fds) {
  if (prefix.empty() || count <= 0) {
    return;
  }

  if (Available()) {
    // already initialized
    return;
  }

  g_procs.reset(new LightProcess[count]);
  g_procsCount = count;

  for (int i = 0; i < count; i++) {
    if (!g_procs[i].initShadow(prefix, i, inherited_fds)) {
      for (int j = 0; j < i; j++) {
        g_procs[j].closeShadow();
      }
      g_procs.reset();
      g_procsCount = 0;
      break;
    }
  }

  if (!s_handlerInited) {
    struct sigaction sa;
    struct sigaction old_sa;
    sa.sa_sigaction = &LightProcess::SigChldHandler;
    sa.sa_flags = SA_SIGINFO | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, &old_sa) != 0) {
      Logger::Error("Couldn't install SIGCHLD handler");
      abort();
    }
    s_handlerInited = true;
  }
}