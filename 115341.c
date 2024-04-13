inline size_t Reference::parse_core(const char *s, size_t n, SemanticValues &sv,
                                    Context &c, any &dt) const {
  if (rule_) {
    // Reference rule
    if (rule_->is_macro) {
      // Macro
      FindReference vis(c.top_args(), c.rule_stack.back()->params);

      // Collect arguments
      std::vector<std::shared_ptr<Ope>> args;
      for (auto arg : args_) {
        arg->accept(vis);
        args.emplace_back(std::move(vis.found_ope));
      }

      c.push_args(std::move(args));
      auto se = make_scope_exit([&]() { c.pop_args(); });
      auto ope = get_core_operator();
      return ope->parse(s, n, sv, c, dt);
    } else {
      // Definition
      c.push_args(std::vector<std::shared_ptr<Ope>>());
      auto se = make_scope_exit([&]() { c.pop_args(); });
      auto ope = get_core_operator();
      return ope->parse(s, n, sv, c, dt);
    }
  } else {
    // Reference parameter in macro
    const auto &args = c.top_args();
    return args[iarg_]->parse(s, n, sv, c, dt);
  }
}