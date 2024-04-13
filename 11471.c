void ciEnv::dump_replay_data(int compile_id) {
  static char buffer[O_BUFLEN];
  int ret = jio_snprintf(buffer, O_BUFLEN, "replay_pid%p_compid%d.log", os::current_process_id(), compile_id);
  if (ret > 0) {
    int fd = open(buffer, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd != -1) {
      FILE* replay_data_file = os::open(fd, "w");
      if (replay_data_file != NULL) {
        fileStream replay_data_stream(replay_data_file, /*need_close=*/true);
        dump_replay_data(&replay_data_stream);
        tty->print_cr("# Compiler replay data is saved as: %s", buffer);
      } else {
        tty->print_cr("# Can't open file to dump replay data.");
      }
    }
  }
}