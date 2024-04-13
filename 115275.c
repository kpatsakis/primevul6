  Fty make_adaptor(F fn, R (F::*)(SemanticValues &sv, any &dt)) {
    return TypeAdaptor_sv_dt<R>(fn);
  }