inline bool Context::is_traceable(const Ope &ope) const {
  if (tracer_enter && tracer_leave) {
    IsReference vis;
    const_cast<Ope &>(ope).accept(vis);
    return !vis.is_reference;
  }
  return false;
}