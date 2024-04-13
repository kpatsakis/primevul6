  Fty make_adaptor(F fn, R (*)(SemanticValues &sv, any &dt)) {
    return TypeAdaptor_sv_dt<R>(fn);
  }