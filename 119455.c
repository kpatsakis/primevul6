  Status CheckInputs(const NodeDef* node, const NodeDef* pack,
                     PartialTensorShape* pack_output_shape, int* pack_axis,
                     bool* return_early) {
    *return_early = true;
    TF_RETURN_IF_ERROR(CheckAttrExists(*pack, "axis"));

    *pack_axis = pack->attr().at("axis").i();
    auto slice_properties =
        ctx().graph_properties->GetInputProperties(node->name());
    if (slice_properties.empty() ||
        slice_properties[0].shape().unknown_rank()) {
      return Status::OK();
    }
    *pack_output_shape = slice_properties[0].shape();
    const int pack_output_rank = pack_output_shape->dims();
    if (*pack_axis < 0) {
      *pack_axis += pack_output_rank;
    }
    if (*pack_axis < 0 || *pack_axis >= pack_output_rank) {
      return errors::InvalidArgument(
          "Pack node (", pack->name(),
          ") axis attribute is out of bounds: ", pack->attr().at("axis").i());
    }
    *return_early = false;
    return Status::OK();
  }