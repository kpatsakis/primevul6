  std::pair<size_t, size_t> line_info() const {
    const auto &idx = *source_line_index;

    auto cur = static_cast<size_t>(std::distance(ss, s_));
    auto it = std::lower_bound(
        idx.begin(), idx.end(), cur,
        [](size_t element, size_t value) { return element < value; });

    auto id = static_cast<size_t>(std::distance(idx.begin(), it));
    auto off = cur - (id == 0 ? 0 : idx[id - 1] + 1);
    return std::make_pair(id + 1, off + 1);
  }