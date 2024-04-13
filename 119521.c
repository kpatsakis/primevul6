    bool operator<(const ChainLink& other) const {
      if (port_origin < other.port_origin) {
        return true;
      } else if (port_origin > other.port_origin) {
        return false;
      } else {
        return node->name() < other.node->name();
      }
    }