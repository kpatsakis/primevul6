  Status GetSimpleSliceAxis(const NodeDef* node, const NodeDef* pack,
                            const PartialTensorShape& pack_output_shape,
                            int pack_axis, int64* slice_start_value,
                            bool* found) {
    NodeDef* slice_begin;
    NodeDef* slice_size;
    TF_RETURN_IF_ERROR(GetInputNode(node->input(1), &slice_begin));
    TF_RETURN_IF_ERROR(GetInputNode(node->input(2), &slice_size));
    for (const auto* n : {slice_begin, slice_size}) {
      if (!IsReallyConstant(*n)) return Status::OK();
    }

    Tensor slice_begin_t;
    Tensor slice_size_t;
    TF_RETURN_IF_ERROR(CheckAttrExists(*slice_begin, "value"));
    if (!slice_begin_t.FromProto(slice_begin->attr().at("value").tensor())) {
      return Status::OK();
    }
    TF_RETURN_IF_ERROR(CheckAttrExists(*slice_size, "value"));
    if (!slice_size_t.FromProto(slice_size->attr().at("value").tensor())) {
      return Status::OK();
    }

    auto copy_tensor_values_to_vector =
        [node](const Tensor& t, gtl::InlinedVector<int64, 4>* vec) {
          if (t.dtype() == DT_INT32) {
            auto t_flat = t.flat<int32>();
            vec->assign(&t_flat(0), &t_flat(t.NumElements()));
          } else if (t.dtype() == DT_INT64) {
            auto t_flat = t.flat<int64>();
            vec->assign(&t_flat(0), &t_flat(t.NumElements()));
          } else {
            return errors::InvalidArgument("Node ", node->name(),
                                           " has invalid type for Index attr: ",
                                           DataTypeString(t.dtype()));
          }
          return Status::OK();
        };

    gtl::InlinedVector<int64, 4> slice_begin_vec;
    gtl::InlinedVector<int64, 4> slice_size_vec;
    TF_RETURN_IF_ERROR(
        copy_tensor_values_to_vector(slice_begin_t, &slice_begin_vec));
    TF_RETURN_IF_ERROR(
        copy_tensor_values_to_vector(slice_size_t, &slice_size_vec));

    if (slice_begin_vec.size() != slice_size_vec.size()) {
      return errors::InvalidArgument("Node ", node->name(),
                                     " has mismatched lengths for begin (",
                                     slice_begin_vec.size(), ") and size (",
                                     slice_size_vec.size(), ") vectors.");
    }
    int slice_begin_vec_size = slice_begin_vec.size();
    if (!pack_output_shape.unknown_rank() &&
        slice_begin_vec_size != pack_output_shape.dims()) {
      return Status::OK();
    }
    if (pack_axis >= slice_begin_vec_size) {
      return errors::InvalidArgument(
          "Input to node ", node->name(), " had pack_axis ", pack_axis,
          " but rank was ", slice_begin_vec_size, ".");
    }

    *slice_start_value = slice_begin_vec[pack_axis];
    if (slice_size_vec[pack_axis] != 1) {
      // Not slicing a single value out.
      return Status::OK();
    }

    for (int i = 0; i < slice_begin_vec_size; ++i) {
      if (i != pack_axis) {
        if (slice_begin_vec[i] != 0 ||
            !(slice_size_vec[i] == -1 ||
              slice_size_vec[i] == pack_output_shape.dim_size(i))) {
          // Not slicing on the same axis as the Pack op.
          return Status::OK();
        }
      }
    }

    if (*slice_start_value < 0 || *slice_start_value >= pack->input_size()) {
      return errors::InvalidArgument(
          "Node ", node->name(), " requested invalid slice index ",
          *slice_start_value, " on axis ", pack_axis,
          " from tensor of shape: ", pack_output_shape.DebugString());
    }

    *found = true;  // slice_start_value is valid.
    return Status::OK();
  }