  Fty make_adaptor(F fn, R (*)(SemanticValues &sv)) {
    return TypeAdaptor_sv<R>(fn);
  }