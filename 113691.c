pid_t LightProcess::waitpid(pid_t pid, int *stat_loc, int options,
                            int timeout) {
  if (!Available()) {
    // light process is not really there
    return ::waitpid(pid, stat_loc, options);
  }

  int id = GetId();
  Lock lock(g_procs[id].m_procMutex);

  FILE *fout = g_procs[id].m_fout;
  lwp_write(fout, "waitpid");
  lwp_write_int64(fout, (int64_t)pid);
  lwp_write_int32(fout, options);
  lwp_write_int32(fout, timeout);
  fflush(g_procs[id].m_fout);

  int64_t ret;
  int stat;
  FILE *fin = g_procs[id].m_fin;
  lwp_read_int64(fin, ret);
  lwp_read_int32(fin, stat);

  *stat_loc = stat;
  if (ret < 0) {
    lwp_read_int32(fin, errno);
  }
  return (pid_t)ret;
}