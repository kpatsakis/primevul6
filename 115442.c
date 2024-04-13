  Fty make_adaptor(F fn, R (F::*)(const SemanticValues &sv) const) {
    return TypeAdaptor_csv<R>(fn);
  }