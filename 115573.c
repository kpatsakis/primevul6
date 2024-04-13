  Fty make_adaptor(F fn, R (*)(const SemanticValues &sv)) {
    return TypeAdaptor_csv<R>(fn);
  }