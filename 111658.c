  Status ListDevices(std::vector<DeviceAttributes>* response) override {
    return wrapped_->ListDevices(response);
  }