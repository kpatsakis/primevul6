inline size_t Ope::parse(const char *s, size_t n, SemanticValues &sv,
                         Context &c, any &dt) const {
  if (c.is_traceable(*this)) {
    TraceOpeName vis;
    const_cast<Ope &>(*this).accept(vis);
    c.trace_enter(vis.name, s, n, sv, dt);
    auto len = parse_core(s, n, sv, c, dt);
    c.trace_leave(vis.name, s, n, sv, dt, len);
    return len;
  }
  return parse_core(s, n, sv, c, dt);
}