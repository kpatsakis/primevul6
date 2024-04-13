void Assign(const D& d, Out out, Rhs rhs) {
  out.device(d) = rhs;
}