  Fty make_adaptor(F fn, R (F::*)(const SemanticValues &sv, any &dt) const) {
    return TypeAdaptor_csv_dt<R>(fn);
  }