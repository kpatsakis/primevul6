pid_t LightProcess::proc_open(const char *cmd, const std::vector<int> &created,
                              const std::vector<int> &desired,
                              const char *cwd,
                              const std::vector<std::string> &env) {
  int id = GetId();
  Lock lock(g_procs[id].m_procMutex);
  always_assert(Available());
  always_assert(created.size() == desired.size());

  FILE *fout = g_procs[id].m_fout;
  lwp_write(fout, "proc_open");
  lwp_write(fout, cmd);
  lwp_write(fout, cwd ? cwd : "");
  lwp_write_int32(fout, (int)env.size());
  for (unsigned int i = 0; i < env.size(); i++) {
    lwp_write(fout, env[i]);
  }

  lwp_write_int32(fout, (int)created.size());
  for (unsigned int i = 0; i < desired.size(); i++) {
    lwp_write_int32(fout, desired[i]);
  }
  fflush(fout);

  bool error_send = false;
  int save_errno = 0;
  for (unsigned int i = 0; i < created.size(); i++) {
    if (!send_fd(g_procs[id].m_afdt_fd, created[i])) {
      error_send = true;
      save_errno = errno;
      break;
    }
  }

  std::string buf;
  FILE *fin = g_procs[id].m_fin;
  lwp_read(fin, buf);
  if (buf == "error") {
    lwp_read_int32(fin, errno);
    if (error_send) {
      // On this error, the receiver side returns dummy errno,
      // use the sender side errno here.
      errno = save_errno;
    }
    return -1;
  }
  always_assert(buf == "success");
  int64_t pid = -1;
  lwp_read_int64(fin, pid);
  always_assert(pid);
  return (pid_t)pid;
}