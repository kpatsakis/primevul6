  void packrat(const char *a_s, size_t def_id, size_t &len, any &val, T fn) {
    if (!enablePackratParsing) {
      fn(val);
      return;
    }

    auto col = a_s - s;
    auto idx = def_count * static_cast<size_t>(col) + def_id;

    if (cache_registered[idx]) {
      if (cache_success[idx]) {
        auto key = std::make_pair(col, def_id);
        std::tie(len, val) = cache_values[key];
        return;
      } else {
        len = static_cast<size_t>(-1);
        return;
      }
    } else {
      fn(val);
      cache_registered[idx] = true;
      cache_success[idx] = success(len);
      if (success(len)) {
        auto key = std::make_pair(col, def_id);
        cache_values[key] = std::make_pair(len, val);
      }
      return;
    }
  }