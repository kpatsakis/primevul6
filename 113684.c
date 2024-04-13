void LightProcess::runShadow(int fdin, int fdout) {
  FILE *fin = fdopen(fdin, "r");
  FILE *fout = fdopen(fdout, "w");

  std::string buf;

  pollfd pfd[1];
  pfd[0].fd = fdin;
  pfd[0].events = POLLIN;
  while (true) {
    int ret = poll(pfd, 1, -1);
    if (ret < 0 && errno == EINTR) {
      continue;
    }
    if (pfd[0].revents & POLLIN) {
      lwp_read(fin, buf);
      if (buf == "exit") {
        Logger::Info("LightProcess exiting upon request");
        break;
      } else if (buf == "popen") {
        do_popen(fin, fout, m_afdt_fd);
      } else if (buf == "pclose") {
        do_pclose(fin, fout);
      } else if (buf == "proc_open") {
        do_proc_open(fin, fout, m_afdt_fd);
      } else if (buf == "waitpid") {
        do_waitpid(fin, fout);
      } else if (buf == "change_user") {
        do_change_user(fin, fout);
      } else if (buf[0]) {
        Logger::Info("LightProcess got invalid command: %.20s", buf.c_str());
      }
    } else if (pfd[0].revents & POLLHUP) {
      // no more command can come in
      Logger::Error("Lost parent, LightProcess exiting");
      break;
    }
  }

  fclose(fin);
  fclose(fout);
  ::close(m_afdt_fd);
  remove(m_afdtFilename.c_str());
  _Exit(0);
}