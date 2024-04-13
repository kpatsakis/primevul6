bool LightProcess::initShadow(const std::string &prefix, int id,
                              const std::vector<int> &inherited_fds) {
  Lock lock(m_procMutex);

  std::ostringstream os;
  os << prefix << "." << getpid() << "." << id;
  m_afdtFilename = os.str();

  // remove the possible leftover
  remove(m_afdtFilename.c_str());

  afdt_error_t err;
  m_afdt_lfd = afdt_listen(m_afdtFilename.c_str(), &err);
  if (m_afdt_lfd < 0) {
    Logger::Warning("Unable to afdt_listen to %s: %d %s",
                    m_afdtFilename.c_str(),
                    errno, folly::errnoStr(errno).c_str());
    return false;
  }

  CPipe p1, p2;
  if (!p1.open() || !p2.open()) {
    Logger::Warning("Unable to create pipe: %d %s", errno,
                    folly::errnoStr(errno).c_str());
    return false;
  }

  pid_t child = fork();
  if (child == 0) {
    // child
    pid_t sid = setsid();
    if (sid < 0) {
      Logger::Warning("Unable to setsid");
      exit(-1);
    }
    m_afdt_fd = afdt_connect(m_afdtFilename.c_str(), &err);
    if (m_afdt_fd < 0) {
      Logger::Warning("Unable to afdt_connect, filename %s: %d %s",
                      m_afdtFilename.c_str(),
                      errno, folly::errnoStr(errno).c_str());
      exit(-1);
    }
    int fd1 = p1.detachOut();
    int fd2 = p2.detachIn();
    p1.close();
    p2.close();

    // don't hold on to previous light processes' pipes, inherited
    // fds, or the afdt listening socket
    for (int i = 0; i < id; i++) {
      g_procs[i].closeFiles();
    }
    close_fds(inherited_fds);
    ::close(m_afdt_lfd);

    runShadow(fd1, fd2);
  } else if (child < 0) {
    // failed
    Logger::Warning("Unable to fork lightly: %d %s", errno,
                    folly::errnoStr(errno).c_str());
    return false;
  } else {
    // parent
    m_fin = fdopen(p2.detachOut(), "r");
    m_fout = fdopen(p1.detachIn(), "w");
    m_shadowProcess = child;

    sockaddr addr;
    socklen_t addrlen = sizeof(addr);
    m_afdt_fd = accept(m_afdt_lfd, &addr, &addrlen);
    if (m_afdt_fd < 0) {
      Logger::Warning("Unable to establish afdt connection: %d %s",
                      errno, folly::errnoStr(errno).c_str());
      closeShadow();
      return false;
    }
  }
  return true;
}