void LightProcess::SigChldHandler(int sig, siginfo_t* info, void* ctx) {
  if (info->si_code != CLD_EXITED &&
      info->si_code != CLD_KILLED &&
      info->si_code != CLD_DUMPED) {
    return;
  }
  pid_t pid = info->si_pid;
  for (int i = 0; i < g_procsCount; ++i) {
    if (g_procs && g_procs[i].m_shadowProcess == pid) {
      // The exited process was a light process. Notify the callback, if any.
      if (s_lostChildHandler) {
        s_lostChildHandler(pid);
      }
      break;
    }
  }
}