  Status TrySimplify(NodeDef* consumer, string* simplified_node_name) override {
    NodeDef* producer;

    if (consumer->input_size() < 1) {
      return errors::FailedPrecondition("Node ", simplified_node_name,
                                        " lacks inputs");
    }

    TF_RETURN_IF_ERROR(GetInputNode(consumer->input(0), &producer));
    const bool producer_is_cast = IsCastLike(*producer);
    const bool can_optimize =
        !IsCheckNumerics(*producer) &&
        ((producer_is_cast && IsValuePreserving(*consumer)) ||
         (IsValuePreserving(*producer) && IsCastLike(*consumer)));
    if (!can_optimize || IsControlFlow(*producer) ||
        IsInPreserveSet(*producer) ||
        producer->device() != consumer->device()) {
      return Status::OK();
    }

    const NodeDef* cast_like_node = producer_is_cast ? producer : consumer;
    const OpDef* cast_like_op_def = nullptr;
    TF_RETURN_IF_ERROR(OpRegistry::Global()->LookUpOpDef(cast_like_node->op(),
                                                         &cast_like_op_def));
    DataType cast_src_type;
    TF_RETURN_IF_ERROR(InputTypeForNode(*cast_like_node, *cast_like_op_def, 0,
                                        &cast_src_type));
    DataType cast_dst_type;
    TF_RETURN_IF_ERROR(OutputTypeForNode(*cast_like_node, *cast_like_op_def, 0,
                                         &cast_dst_type));
    if (!IsFixedSizeType(cast_src_type) || !IsFixedSizeType(cast_dst_type)) {
      return Status::OK();
    } else if (producer_is_cast &&
               DataTypeSize(cast_dst_type) <= DataTypeSize(cast_src_type)) {
      return Status::OK();
    } else if (!producer_is_cast &&
               DataTypeSize(cast_dst_type) >= DataTypeSize(cast_src_type)) {
      return Status::OK();
    }

    // Check that nodes were not already optimized.
    const string optimized_producer_name = OptimizedNodeName(
        ParseNodeScopeAndName(producer->name()), DataTypeString(cast_dst_type));
    const string optimized_consumer_name = OptimizedNodeName(
        ParseNodeScopeAndName(consumer->name()), DataTypeString(cast_src_type));
    const bool is_already_optimized =
        ctx().node_map->NodeExists(optimized_consumer_name) ||
        ctx().node_map->NodeExists(optimized_producer_name);
    if (is_already_optimized) {
      return Status::OK();
    }

    // Add copies of consumer and producer in reverse order.
    NodeDef* input;
    TF_RETURN_IF_ERROR(GetInputNode(producer->input(0), &input));
    // Create new producer node.
    NodeDef* new_producer = AddCopyNode(optimized_consumer_name, consumer);
    new_producer->set_input(0, producer->input(0));
    ctx().node_map->AddOutput(input->name(), new_producer->name());

    // Create new consumer node.
    NodeDef* new_consumer = AddCopyNode(optimized_producer_name, producer);
    new_consumer->set_input(0, new_producer->name());

    NodeDef* new_value_preserving =
        producer_is_cast ? new_producer : new_consumer;
    const DataType new_input_type =
        producer_is_cast ? cast_src_type : cast_dst_type;
    // Update the input type of the value-preserving node. The input and
    // output types of the cast-like nodes remain the same.
    TF_RETURN_IF_ERROR(SetInputType(new_input_type, new_value_preserving));
    // Make sure there is a kernel registered for the value preserving op
    // with the new input type.
    TF_RETURN_IF_ERROR(IsKernelRegisteredForNode(*new_value_preserving));
    ctx().node_map->AddOutput(new_producer->name(), new_consumer->name());

    AddToOptimizationQueue(new_producer);
    *simplified_node_name = new_consumer->name();

    return Status::OK();
  }