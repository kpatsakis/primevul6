  Fty make_adaptor(F fn, R (F::*)(SemanticValues &sv)) {
    return TypeAdaptor_sv<R>(fn);
  }