inline void Context::trace_leave(const char *name, const char *a_s, size_t n,
                                 SemanticValues &sv, any &dt,
                                 size_t len) const {
  tracer_leave(name, a_s, n, sv, *this, dt, len);
  trace_ids.pop_back();
}