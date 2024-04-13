  const internal::Arg *find(const fmt::BasicStringRef<Char> &name) const {
    // The list is unsorted, so just return the first matching name.
    for (typename MapType::const_iterator it = map_.begin(), end = map_.end();
         it != end; ++it) {
      if (it->first == name)
        return &it->second;
    }
    return FMT_NULL;
  }