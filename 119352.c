  bool NodeIsOnCpuOrGpu(const NodeDef* node) const {
    using absl::StrContains;

    string task;
    string device;

    return DeviceNameUtils::SplitDeviceName(node->device(), &task, &device) &&
           (StrContains(device, DEVICE_CPU) || StrContains(device, DEVICE_GPU));
  }