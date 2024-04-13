    TypeAdaptor_csv_dt(std::function<R(const SemanticValues &sv, any &dt)> fn)
        : fn_(fn) {}