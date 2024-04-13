static void kill_handler(int sig) {
  if (sig == SIGALRM && waited) {
    kill(waited, SIGKILL);
  }
}