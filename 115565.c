  Fty make_adaptor(F fn, R (F::*)(const SemanticValues &sv)) {
    return TypeAdaptor_csv<R>(fn);
  }