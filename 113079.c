HTTP2Codec::mapPriorityToDependency(uint8_t priority) const {
  // If the priority is out of the maximum index of virtual nodes array, we
  // return the lowest level virtual node as a punishment of not setting
  // priority correctly.
  return virtualPriorityNodes_.empty()
    ? 0
    : virtualPriorityNodes_[
        std::min(priority, uint8_t(virtualPriorityNodes_.size() - 1))];
}