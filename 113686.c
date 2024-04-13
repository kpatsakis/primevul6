void LightProcess::ChangeUser(const std::string &username) {
  if (username.empty()) return;
  for (int i = 0; i < g_procsCount; i++) {
    Lock lock(g_procs[i].m_procMutex);
    FILE *fout = g_procs[i].m_fout;
    lwp_write(fout, "change_user");
    lwp_write(fout, username);
    fflush(fout);
  }
}