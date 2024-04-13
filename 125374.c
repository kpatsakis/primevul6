DocumentSourceGroup::DocumentSourceGroup(const intrusive_ptr<ExpressionContext>& expCtx,
                                         boost::optional<size_t> maxMemoryUsageBytes)
    : DocumentSource(kStageName, expCtx),
      _doingMerge(false),
      _memoryTracker{expCtx->allowDiskUse && !expCtx->inMongos,
                     maxMemoryUsageBytes ? *maxMemoryUsageBytes
                                         : internalDocumentSourceGroupMaxMemoryBytes.load()},
      // We spill to disk in debug mode, regardless of allowDiskUse, to stress the system.
      _file(
          !expCtx->inMongos && (expCtx->allowDiskUse || kDebugBuild)
              ? std::make_shared<Sorter<Value, Value>::File>(expCtx->tempDir + "/" + nextFileName())
              : nullptr),
      _initialized(false),
      _groups(expCtx->getValueComparator().makeUnorderedValueMap<Accumulators>()),
      _spilled(false) {}