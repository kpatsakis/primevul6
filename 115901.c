  static std::pair<int64_t, int64_t> GetBiasDimAndSliceSize(
      ArrayRef<int64_t> filter_shape, AffineOpType op) {
    // Channel dimension index is specified as op property
    auto channel_index_iter = filter_shape.begin();
    std::advance(channel_index_iter, op.GetChannelDimIndex());
    // The slide size is the size of the data in higher dimensions.
    int64_t slice_size =
        std::accumulate(std::next(channel_index_iter), filter_shape.end(), 1,
                        std::multiplies<int64_t>());
    return {*channel_index_iter, slice_size};
  }