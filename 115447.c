  void output_log(const char *s, size_t n, const Definition::Result &r) const {
    if (log) {
      if (!r.ret) {
        if (r.message_pos) {
          auto line = line_info(s, r.message_pos);
          log(line.first, line.second, r.message);
        } else {
          auto line = line_info(s, r.error_pos);
          log(line.first, line.second, "syntax error");
        }
      } else if (r.len != n) {
        auto line = line_info(s, s + r.len);
        log(line.first, line.second, "syntax error");
      }
    }
  }