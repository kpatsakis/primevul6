static void do_popen(FILE *fin, FILE *fout, int afdt_fd) {
  std::string buf;
  std::string cwd;

  lwp_read(fin, buf);
  bool read_only = (buf[0] == 'r');

  lwp_read(fin, buf);

  std::string old_cwd = Process::GetCurrentDirectory();
  lwp_read(fin, cwd);

  if (old_cwd != cwd) {
    if (chdir(cwd.c_str())) {
      // Ignore chdir failures, because the compiled version might not have the
      // directory any more.
      Logger::Warning("Light Process failed chdir to %s.", cwd.c_str());
    }
  }

  FILE *f = buf[0] ? ::popen(buf.c_str(), read_only ? "r" : "w") : nullptr;

  if (old_cwd != cwd && chdir(old_cwd.c_str())) {
    // only here if we can't change the cwd back
  }

  if (f == nullptr) {
    Logger::Error("Light process failed popen: %d (%s).", errno,
                  folly::errnoStr(errno).c_str());
    lwp_write(fout, "error");
  } else {
    lwp_write(fout, "success");
    lwp_write_int64(fout, (int64_t)f);
    int fd = fileno(f);
    send_fd(afdt_fd, fd);
  }
}