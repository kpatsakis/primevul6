  bool insert(TFE_Py_ForwardAccumulator* element) {
    if (map_.find(element) != map_.end()) {
      return false;
    }
    ListType::iterator it = ordered_.insert(ordered_.end(), element);
    map_.insert(std::make_pair(element, it));
    return true;
  }