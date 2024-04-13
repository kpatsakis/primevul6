    TypeAdaptor_sv_dt(std::function<R(SemanticValues &sv, any &dt)> fn)
        : fn_(fn) {}