  virtual bool cellular_enabled() const {
    return enabled_devices_ & (1 << TYPE_CELLULAR);
  }
