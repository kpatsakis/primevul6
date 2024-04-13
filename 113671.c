static void do_proc_open(FILE *fin, FILE *fout, int afdt_fd) {
  std::string cmd;
  lwp_read(fin, cmd);

  std::string cwd;
  lwp_read(fin, cwd);

  std::string buf;
  int env_size = 0;
  std::vector<std::string> env;
  lwp_read_int32(fin, env_size);
  for (int i = 0; i < env_size; i++) {
    lwp_read(fin, buf);
    env.push_back(buf);
  }

  int pipe_size = 0;
  lwp_read_int32(fin, pipe_size);
  std::vector<int> pvals;
  for (int i = 0; i < pipe_size; i++) {
    int fd_value;
    lwp_read_int32(fin, fd_value);
    pvals.push_back(fd_value);
  }

  std::vector<int> pkeys;
  for (int i = 0; i < pipe_size; i++) {
    int fd = recv_fd(afdt_fd);
    if (fd < 0) {
      lwp_write(fout, "error");
      lwp_write_int32(fout, EPROTO);
      fflush(fout);
      close_fds(pkeys);
      return;
    }
    pkeys.push_back(fd);
  }

  // indicate error if an empty command was received
  if (cmd.length() == 0) {
    lwp_write(fout, "error");
    lwp_write_int32(fout, ENOENT);
    return;
  }

  // now ready to start the child process
  pid_t child = fork();
  if (child == 0) {
    for (int i = 0; i < pipe_size; i++) {
      dup2(pkeys[i], pvals[i]);
    }
    if (cwd.length() > 0 && chdir(cwd.c_str())) {
      // non-zero for error
      // chdir failed, the working directory remains unchanged
    }
    if (!env.empty()) {
      char **envp = build_envp(env);
      execle("/bin/sh", "sh", "-c", cmd.c_str(), nullptr, envp);
      free(envp);
    } else {
      execl("/bin/sh", "sh", "-c", cmd.c_str(), nullptr);
    }
    _exit(127);
  } else if (child > 0) {
    // successfully created the child process
    lwp_write(fout, "success");
    lwp_write_int64(fout, (int64_t)child);
    fflush(fout);
  } else {
    // failed creating the child process
    lwp_write(fout, "error");
    lwp_write_int32(fout, errno);
    fflush(fout);
  }

  close_fds(pkeys);
}