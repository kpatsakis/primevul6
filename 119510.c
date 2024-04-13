  Status GetSliceAxis(const NodeDef* node, const NodeDef* pack,
                      const PartialTensorShape& pack_output_shape,
                      int pack_axis, int64* slice_start_value, bool* found,
                      bool* must_expand_dims) {
    *found = false;
    if (IsSlice(*node)) {
      *must_expand_dims = true;
      return GetSimpleSliceAxis(node, pack, pack_output_shape, pack_axis,
                                slice_start_value, found);
    } else {
      return GetStridedSliceAxis(node, pack, pack_output_shape, pack_axis,
                                 slice_start_value, found, must_expand_dims);
    }
  }