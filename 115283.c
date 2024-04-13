  Fty make_adaptor(F fn, R (F::*)(SemanticValues &sv) const) {
    return TypeAdaptor_sv<R>(fn);
  }