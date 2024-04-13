void LightProcess::closeFiles() {
  fclose(m_fin);
  fclose(m_fout);
  ::close(m_afdt_fd);
  ::close(m_afdt_lfd);
}