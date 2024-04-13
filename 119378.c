  Status TrySimplify(NodeDef* node, string* simplified_node_name) override {
    NodeDef *input, *ones;
    TF_RETURN_IF_ERROR(GetInputNode(node->input(0), &input));
    TF_RETURN_IF_ERROR(GetInputNode(node->input(1), &ones));
    if (IsInPreserveSet(*node) || IsInPreserveSet(*input) ||
        IsInPreserveSet(*ones)) {
      return Status::OK();
    }

    // TODO(kkiningh): Generalize using IsOnes from constant_folding.cc
    if (IsConstant(*input) || !IsOnes(*ones)) return Status::OK();

    // Avoid optimizing the same node twice
    const NodeScopeAndName scope_and_name = ParseNodeScopeAndName(node->name());
    const string tile_node_name = OptimizedNodeName(scope_and_name, "Tile");
    const string const_node_name = OptimizedNodeName(scope_and_name, "Const");
    if (ctx().node_map->NodeExists(tile_node_name) ||
        ctx().node_map->NodeExists(const_node_name)) {
      return Status::OK();
    }

    const std::vector<OpInfo::TensorProperties>& props =
        ctx().graph_properties->GetInputProperties(node->name());
    if (props.size() != 2) return Status::OK();

    // Ignore ops where the shape doesn't change
    const TensorShapeProto& input_shape = props[0].shape();
    const TensorShapeProto& ones_shape = props[1].shape();
    TensorShapeProto output_shape;
    if (!ShapeAfterBroadcast(input_shape, ones_shape, &output_shape)) {
      return Status::OK();
    }
    if (ShapesSymbolicallyEqual(input_shape, output_shape)) {
      return Status::OK();
    }

    // All inputs must have same input/output dimensions
    if (input_shape.dim_size() != output_shape.dim_size() ||
        ones_shape.dim_size() != output_shape.dim_size())
      return Status::OK();

    // At this point all preconditions are met. Can proceed with rewrite.
    VLOG(3) << "Simplify multiply with all ones input: node=" << node->name()
            << "@" << output_shape << " ones=" << ones->name() << "@"
            << ones_shape << " input=" << input->name() << "@" << input_shape;

    // 1. Create constant node with correct tile multiples
    Tensor multiples(DT_INT32, TensorShape({output_shape.dim_size()}));
    for (int i = 0; i < output_shape.dim_size(); ++i) {
      int64 size = output_shape.dim(i).size() / input_shape.dim(i).size();
      if (TF_PREDICT_FALSE(size >= INT_MAX)) {
        return Status(error::OUT_OF_RANGE, "int32 overflow");
      }
      multiples.flat<int32>()(i) = static_cast<int32>(size);
    }

    NodeDef* const_node = AddEmptyNode(const_node_name);
    TF_RETURN_IF_ERROR(ConstantFolding::CreateNodeDef(
        const_node->name(), TensorValue(&multiples), const_node));
    const_node->set_device(node->device());
    ForwardControlDependencies(const_node, {ones});
    AddToOptimizationQueue(const_node);

    // 2. Replace multiply node with Tile(Const, input);
    const DataType type = GetDataTypeFromAttr(*node, "T");
    NodeDef* tile_node = AddEmptyNode(tile_node_name);
    tile_node->set_op("Tile");
    tile_node->set_device(node->device());
    SetDataTypeToAttr(type, "T", tile_node);
    SetDataTypeToAttr(DT_INT32, "Tmultiples", tile_node);
    tile_node->add_input(input->name());
    tile_node->add_input(const_node->name());

    ForwardControlDependencies(tile_node, {node});
    *simplified_node_name = tile_node->name();

    return Status::OK();
  }