  Status GetStridedSliceAxis(const NodeDef* node, const NodeDef* pack,
                             const PartialTensorShape& pack_output_shape,
                             int pack_axis, int64* slice_start_value,
                             bool* found, bool* must_expand_dims) {
    TF_RETURN_IF_ERROR(
        CheckAttrsExist(*node, {"begin_mask", "end_mask", "ellipsis_mask",
                                "new_axis_mask", "shrink_axis_mask"}));

    const int begin_mask = node->attr().at("begin_mask").i();
    const int end_mask = node->attr().at("end_mask").i();
    const int ellipsis_mask = node->attr().at("ellipsis_mask").i();
    const int new_axis_mask = node->attr().at("new_axis_mask").i();
    const int shrink_axis_mask = node->attr().at("shrink_axis_mask").i();

    // Check that the StridedSlice is one of these at pack_axis:
    //   [..., i, ...]
    //   [..., i:i+1, ...]
    //   [..., :1, ...]
    //   [..., -1:, ...]
    ///  [..., s_{pack_axis}-1:, ...]
    NodeDef* slice_begin;
    NodeDef* slice_end;
    NodeDef* slice_strides;
    TF_RETURN_IF_ERROR(GetInputNode(node->input(1), &slice_begin));
    TF_RETURN_IF_ERROR(GetInputNode(node->input(2), &slice_end));
    TF_RETURN_IF_ERROR(GetInputNode(node->input(3), &slice_strides));

    for (const auto* n : {slice_begin, slice_end, slice_strides}) {
      if (!IsReallyConstant(*n)) return Status::OK();
    }

    Tensor slice_begin_t;
    Tensor slice_end_t;
    Tensor slice_strides_t;

    TF_RETURN_IF_ERROR(CheckAttrExists(*slice_begin, "value"));
    if (!slice_begin_t.FromProto(slice_begin->attr().at("value").tensor())) {
      return Status::OK();
    }
    TF_RETURN_IF_ERROR(CheckAttrExists(*slice_end, "value"));
    if (!slice_end_t.FromProto(slice_end->attr().at("value").tensor())) {
      return Status::OK();
    }
    TF_RETURN_IF_ERROR(CheckAttrExists(*slice_strides, "value"));
    if (!slice_strides_t.FromProto(
            slice_strides->attr().at("value").tensor())) {
      return Status::OK();
    }
    TensorShape processing_shape;
    TensorShape final_shape;
    bool is_identity;
    bool is_simple_slice;
    bool slice_dim0;
    gtl::InlinedVector<int64, 4> slice_begin_vec;
    gtl::InlinedVector<int64, 4> slice_end_vec;
    gtl::InlinedVector<int64, 4> slice_strides_vec;
    TF_RETURN_IF_ERROR(ValidateStridedSliceOp(
        &slice_begin_t, &slice_end_t, slice_strides_t, pack_output_shape,
        begin_mask, end_mask, ellipsis_mask, new_axis_mask, shrink_axis_mask,
        &processing_shape, &final_shape, &is_identity, &is_simple_slice,
        &slice_dim0, &slice_begin_vec, &slice_end_vec, &slice_strides_vec));

    if (!is_simple_slice) return Status::OK();

    int begin_index = -1;
    int64 begin_value = 0;
    for (int i = 0, end = slice_begin_vec.size(); i < end; ++i) {
      const int64 v = slice_begin_vec[i];
      if (v != 0) {
        if (begin_index != -1) {
          // At least two start values that are nonzero.
          return Status::OK();
        }
        begin_index = i;
        begin_value = v;
      }
    }

    int end_index = -1;
    int64 end_value = 0;
    for (int i = 0, end = slice_begin_vec.size(); i < end; ++i) {
      const int64 v = slice_end_vec[i];
      if (v != pack_output_shape.dim_size(i)) {
        if (end_index != -1) {
          // At least two end values that are nonzero.
          return Status::OK();
        }
        end_index = i;
        end_value = v;
      }
    }

    if (begin_index == -1 && end_index == -1) return Status::OK();
    if (begin_index != -1 && end_index != -1 && begin_index != end_index) {
      // Somehow received different axes for begin/end slicing
      return Status::OK();
    }
    const int slice_axis = (begin_index == -1) ? end_index : begin_index;
    if (slice_axis != pack_axis) {
      // Not slicing on the same axis as the Pack op.
      return Status::OK();
    }
    *slice_start_value = (begin_index == -1) ? 0 : begin_value;
    const int64 slice_end_value =
        (end_index == -1) ? pack_output_shape.dim_size(slice_axis) : end_value;
    if (slice_end_value != *slice_start_value + 1) {
      // Not slicing a single value out.
      return Status::OK();
    }

    if (*slice_start_value < 0 || *slice_start_value >= pack->input_size()) {
      return errors::InvalidArgument(
          "Node ", node->name(), " requested invalid slice index ",
          *slice_start_value, " on axis ", slice_axis,
          " from tensor of shape: ", pack_output_shape.DebugString());
    }

    if (shrink_axis_mask == 0) {
      *must_expand_dims = true;
    } else if (shrink_axis_mask == (1 << slice_axis)) {
      *must_expand_dims = false;
    } else {
      // Shrinking on a different axis from the one that we are slicing on.
      return Status::OK();
    }

    *found = true;  // slice_start_value is valid.
    return Status::OK();
  }