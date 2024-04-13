  virtual bool ethernet_enabled() const {
    return enabled_devices_ & (1 << TYPE_ETHERNET);
  }
