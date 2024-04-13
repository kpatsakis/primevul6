  Result parse_core(const char *s, size_t n, SemanticValues &sv, any &dt,
                    const char *path) const {
    initialize_definition_ids();

    std::shared_ptr<Ope> ope = holder_;
    if (whitespaceOpe) { ope = std::make_shared<Sequence>(whitespaceOpe, ope); }

    Context cxt(path, s, n, definition_ids_.size(), whitespaceOpe, wordOpe,
                enablePackratParsing, tracer_enter, tracer_leave);

    auto len = ope->parse(s, n, sv, cxt, dt);
    return Result{success(len), len, cxt.error_pos, cxt.message_pos,
                  cxt.message};
  }