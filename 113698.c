static void do_waitpid(FILE *fin, FILE *fout) {
  int64_t p = -1;
  int options = 0;
  int timeout = 0;
  lwp_read_int64(fin, p);
  lwp_read_int32(fin, options);
  lwp_read_int32(fin, timeout);

  pid_t pid = (pid_t)p;
  int stat;
  if (timeout > 0) {
    waited = pid;
    signal(SIGALRM, kill_handler);
    alarm(timeout);
  }

  pid_t ret = ::waitpid(pid, &stat, options);
  alarm(0); // cancel the previous alarm if not triggered yet
  waited = 0;
  lwp_write_int64(fout, ret);
  lwp_write_int32(fout, stat);
  if (ret < 0) {
    lwp_write_int32(fout, errno);
  }
  fflush(fout);
}