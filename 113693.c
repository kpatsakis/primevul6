void LightProcess::closeShadow() {
  Lock lock(m_procMutex);
  if (m_shadowProcess) {
    lwp_write(m_fout, "exit");
    fflush(m_fout);
    fclose(m_fin);
    fclose(m_fout);
    // removes the "zombie" process, so not to interfere with later waits
    ::waitpid(m_shadowProcess, nullptr, 0);
  }
  if (!m_afdtFilename.empty()) {
    remove(m_afdtFilename.c_str());
  }
  if (m_afdt_fd >= 0) {
    ::close(m_afdt_fd);
    m_afdt_fd = -1;
  }
  m_shadowProcess = 0;
}