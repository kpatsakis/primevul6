static bool setMinimalCapabilities() {
  cap_t cap_d = cap_init();

  if (cap_d != nullptr) {
    cap_value_t cap_list[] = {CAP_NET_BIND_SERVICE, CAP_SYS_RESOURCE,
                              CAP_SYS_NICE};

    cap_clear(cap_d);

    if (cap_set_flag(cap_d, CAP_PERMITTED, 3, cap_list, CAP_SET) < 0 ||
        cap_set_flag(cap_d, CAP_EFFECTIVE, 3, cap_list, CAP_SET) < 0) {
      Logger::Error("cap_set_flag failed");
      return false;
    }

    if (cap_set_proc(cap_d) == -1) {
      Logger::Error("cap_set_proc failed");
      return false;
    }

    if (cap_free(cap_d) == -1) {
      Logger::Error("cap_free failed");
      return false;
    }

    prctl(PR_SET_DUMPABLE, 1, 0, 0, 0);
    return true;
  }
  return false;
}