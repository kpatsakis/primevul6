  Status TrySimplify(NodeDef* node, string* simplified_node_name) override {
    NodeDef* tile;
    TF_RETURN_IF_ERROR(GetInputNode(node->input(0), &tile));
    if (!IsTile(*tile) || IsInPreserveSet(*tile)) {
      return Status::OK();
    }

    if (NumNonControlOutputs(*tile, *ctx().node_map) != 1) {
      // Optimization is only worthwile when there is a single output from Tile.
      // Otherwise, we need to insert addtional Reshape ops that can't be easily
      // removed.
      return Status::OK();
    }

    NodeDef* reshape;
    TF_RETURN_IF_ERROR(GetInputNode(tile->input(0), &reshape));
    if (!IsReshape(*reshape) || IsInPreserveSet(*reshape)) {
      return Status::OK();
    }

    NodeDef* multiples;
    TF_RETURN_IF_ERROR(GetInputNode(tile->input(1), &multiples));

    NodeDef* shape;
    TF_RETURN_IF_ERROR(GetInputNode(reshape->input(1), &shape));

    // Avoid optimizing the same nodes twice
    const NodeScopeAndName scope_and_name = ParseNodeScopeAndName(node->name());
    const string new_reshape_name =
        OptimizedNodeName(scope_and_name, "Reshape");
    const string new_tile_name = OptimizedNodeName(scope_and_name, "Tile");
    const string new_multiples_name =
        OptimizedNodeName(scope_and_name, "Multiples");
    const string new_shape_name = OptimizedNodeName(scope_and_name, "Shape");
    if (ctx().node_map->NodeExists(new_reshape_name) ||
        ctx().node_map->NodeExists(new_tile_name) ||
        ctx().node_map->NodeExists(new_shape_name) ||
        ctx().node_map->NodeExists(new_multiples_name)) {
      return Status::OK();
    }

    // Compuate updated multiples/shape values.
    AttrValue new_multiples_attr;
    if (!CreateUpdatedMultiplesProto(multiples,
                                     new_multiples_attr.mutable_tensor())) {
      return Status::OK();
    }
    AttrValue new_shape_attr;
    if (!CreateUpdatedShapeProto(shape, new_shape_attr.mutable_tensor())) {
      return Status::OK();
    }

    // At this point the graph is validated and can be updated
    // Note: We can assume shape/multiples are DT_INT32 ony at this point since
    // they're checked in CreateUpdated*Proto()

    // 1. Create the constant nodes used by the new Reshape/Tile nodes
    NodeDef* new_multiples = AddEmptyNode(new_multiples_name);
    new_multiples->set_op("Const");
    SetDataTypeToAttr(DT_INT32, "dtype", new_multiples);
    new_multiples->mutable_attr()->insert({"value", new_multiples_attr});
    new_multiples->set_device(multiples->device());

    NodeDef* new_shape = AddEmptyNode(new_shape_name);
    new_shape->set_op("Const");
    SetDataTypeToAttr(DT_INT32, "dtype", new_shape);
    new_shape->mutable_attr()->insert({"value", new_shape_attr});
    new_shape->set_device(shape->device());

    // 2. Create the new Reshape/Tile nodes
    NodeDef* new_reshape = AddEmptyNode(new_reshape_name);
    CopyReshapeWithInput(reshape, new_reshape, /*input=*/reshape->input(0),
                         /*shape=*/new_shape->name());
    NodeDef* new_tile = AddEmptyNode(new_tile_name);
    CopyTileWithInput(tile, new_tile, /*input=*/new_reshape->name(),
                      /*multiples=*/new_multiples->name());

    // 3. Update consumer of original Tile node and add control
    node->set_input(0, new_tile->name());
    ctx().node_map->UpdateInput(node->name(), tile->name(), new_tile->name());

    ForwardControlDependencies(new_tile, {tile});
    ForwardControlDependencies(new_multiples, {multiples});
    ForwardControlDependencies(new_reshape, {reshape});
    ForwardControlDependencies(new_shape, {shape});

    *simplified_node_name = node->name();
    return Status::OK();
  }