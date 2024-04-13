  Context(const char *a_path, const char *a_s, size_t a_l, size_t a_def_count,
          std::shared_ptr<Ope> a_whitespaceOpe, std::shared_ptr<Ope> a_wordOpe,
          bool a_enablePackratParsing, TracerEnter a_tracer_enter,
          TracerLeave a_tracer_leave)
      : path(a_path), s(a_s), l(a_l), whitespaceOpe(a_whitespaceOpe),
        wordOpe(a_wordOpe), def_count(a_def_count),
        enablePackratParsing(a_enablePackratParsing),
        cache_registered(enablePackratParsing ? def_count * (l + 1) : 0),
        cache_success(enablePackratParsing ? def_count * (l + 1) : 0),
        tracer_enter(a_tracer_enter), tracer_leave(a_tracer_leave) {

    for (size_t pos = 0; pos < l; pos++) {
      if (s[pos] == '\n') { source_line_index.push_back(pos); }
    }
    source_line_index.push_back(l);

    args_stack.resize(1);

    push_capture_scope();
  }