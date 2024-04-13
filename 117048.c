  void erase(TFE_Py_ForwardAccumulator* element) {
    MapType::iterator existing = map_.find(element);
    if (existing == map_.end()) {
      return;
    }
    ListType::iterator list_position = existing->second;
    map_.erase(existing);
    ordered_.erase(list_position);
  }