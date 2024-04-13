  SafeVariableWatcherSet()
      : SafeSetCopy<
            tensorflow::gtl::CompactPointerSet<TFE_Py_VariableWatcher*>>(
            *GetVariableWatcherSet()) {}