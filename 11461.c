void ciEnv::dump_inline_data(int compile_id) {
  static char buffer[O_BUFLEN];
  int ret = jio_snprintf(buffer, O_BUFLEN, "inline_pid%p_compid%d.log", os::current_process_id(), compile_id);
  if (ret > 0) {
    int fd = open(buffer, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd != -1) {
      FILE* inline_data_file = os::open(fd, "w");
      if (inline_data_file != NULL) {
        fileStream replay_data_stream(inline_data_file, /*need_close=*/true);
        GUARDED_VM_ENTRY(
          MutexLocker ml(Compile_lock);
          dump_compile_data(&replay_data_stream);
        )
        replay_data_stream.flush();
        tty->print("# Compiler inline data is saved as: ");
        tty->print_cr("%s", buffer);
      } else {
        tty->print_cr("# Can't open file to dump inline data.");
      }
    }
  }
}