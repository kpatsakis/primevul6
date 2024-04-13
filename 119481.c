bool NodeIsOnCpu(const NodeDef& node) {
  string task;
  string device;
  return DeviceNameUtils::SplitDeviceName(node.device(), &task, &device) &&
         absl::StrContains(device, DEVICE_CPU);
}