static void close_fds(const std::vector<int> &fds) {
  for (unsigned int i = 0; i < fds.size(); i++) {
    ::close(fds[i]);
  }
}