int LightProcess::pclose(FILE *f) {
  if (!Available()) {
    return ::pclose(f);
  }

  int id = GetId();
  Lock lock(g_procs[id].m_procMutex);

  std::map<int64_t, int64_t>::iterator it = g_procs[id].m_popenMap.find((int64_t)f);
  if (it == g_procs[id].m_popenMap.end()) {
    // try to close it with normal pclose
    return ::pclose(f);
  }

  int64_t f2 = it->second;
  g_procs[id].m_popenMap.erase((int64_t)f);
  fclose(f);

  lwp_write(g_procs[id].m_fout, "pclose");
  lwp_write_int64(g_procs[id].m_fout, f2);

  int ret = -1;
  lwp_read_int32(g_procs[id].m_fin, ret);
  if (ret < 0) {
    lwp_read_int32(g_procs[id].m_fin, errno);
  }
  return ret;
}