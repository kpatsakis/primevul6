  SafeTapeSet()
      : SafeSetCopy<tensorflow::gtl::CompactPointerSet<TFE_Py_Tape*>>(
            *GetTapeSet()) {}