    any operator()(SemanticValues &sv, any & /*dt*/) {
      return call<R>(fn_, sv);
    }