inline void Context::trace_enter(const char *name, const char *a_s, size_t n,
                                 SemanticValues &sv, any &dt) const {
  trace_ids.push_back(next_trace_id++);
  tracer_enter(name, a_s, n, sv, *this, dt);
}