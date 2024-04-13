FILE *LightProcess::LightPopenImpl(const char *cmd, const char *type,
                                   const char *cwd) {
  int id = GetId();
  Lock lock(g_procs[id].m_procMutex);

  FILE *fout = g_procs[id].m_fout;
  lwp_write(fout, "popen");
  lwp_write(fout, type);
  lwp_write(fout, cmd);
  lwp_write(fout, cwd ? cwd : "");
  fflush(fout);

  std::string buf;
  FILE *fin = g_procs[id].m_fin;
  lwp_read(fin, buf);
  if (buf == "error") {
    return nullptr;
  }

  int64_t fptr = 0;
  lwp_read_int64(fin, fptr);
  if (!fptr) {
    Logger::Error("Light process failed to return the file pointer.");
    return nullptr;
  }

  int fd = recv_fd(g_procs[id].m_afdt_fd);
  if (fd < 0) {
    Logger::Error("Light process failed to send the file descriptor.");
    return nullptr;
  }
  FILE *f = fdopen(fd, type);
  g_procs[id].m_popenMap[(int64_t)f] = fptr;

  return f;
}