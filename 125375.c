bool DocumentSourceGroup::shouldSpillWithAttemptToSaveMemory() {
    if (!_memoryTracker._allowDiskUse &&
        (_memoryTracker.currentMemoryBytes() >
         static_cast<long long>(_memoryTracker._maxAllowedMemoryUsageBytes))) {
        freeMemory();
    }

    if (_memoryTracker.currentMemoryBytes() >
        static_cast<long long>(_memoryTracker._maxAllowedMemoryUsageBytes)) {
        uassert(ErrorCodes::QueryExceededMemoryLimitNoDiskUseAllowed,
                "Exceeded memory limit for $group, but didn't allow external sort."
                " Pass allowDiskUse:true to opt in.",
                _memoryTracker._allowDiskUse);
        _memoryTracker.resetCurrent();
        return true;
    }
    return false;
}