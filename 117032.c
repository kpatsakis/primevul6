    bool operator()(const IdAndVariable& lhs, const IdAndVariable& rhs) const {
      return lhs.id < rhs.id;
    }