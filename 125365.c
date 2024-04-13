size_t DocumentSourceGroup::getMaxMemoryUsageBytes() const {
    return _memoryTracker._maxAllowedMemoryUsageBytes;
}