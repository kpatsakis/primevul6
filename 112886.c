  virtual void moduleInit() {
    HHVM_FE(wddx_add_vars);
    HHVM_FE(wddx_packet_end);
    HHVM_FE(wddx_packet_start);
    HHVM_FE(wddx_serialize_value);
    HHVM_FE(wddx_serialize_vars);
    loadSystemlib();
  }