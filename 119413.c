  Status TrySimplify(NodeDef* node, string* simplified_node_name) override {
    // 1. traverse the chain of Pack ops to get the original input
    NodeDef* input = node;
    std::vector<const NodeDef*> chain;
    while (IsPack(*input) && NumNonControlInputs(*node) > 1 &&
           !IsInPreserveSet(*input)) {
      // Only pack operations with all identical inputs are supported
      if (!AllRegularInputsEqual(*input)) {
        break;
      }
      chain.push_back(input);
      TF_RETURN_IF_ERROR(GetInputNode(input->input(0), &input));
    }

    // Must be at least two Pack operations to consider for replacement
    if (chain.empty()) {
      return Status::OK();
    }

    // Avoid optimizing the same node twice
    const NodeScopeAndName node_scope_and_name =
        ParseNodeScopeAndName(node->name());
    const string new_const_name =
        OptimizedNodeName(node_scope_and_name, "Multiples");
    const string new_tile_name = OptimizedNodeName(node_scope_and_name, "Tile");
    const string new_shape_name =
        OptimizedNodeName(node_scope_and_name, "Shape");
    const string new_reshape_name =
        OptimizedNodeName(node_scope_and_name, "Reshape");
    if (ctx().node_map->NodeExists(new_const_name) ||
        ctx().node_map->NodeExists(new_tile_name) ||
        ctx().node_map->NodeExists(new_shape_name) ||
        ctx().node_map->NodeExists(new_reshape_name)) {
      return Status::OK();
    }

    // 2. Calculate the multiples and shape tensor using the chain
    const OpInfo::TensorProperties* input_props;
    TF_RETURN_IF_ERROR(GetTensorProperties(input->name(), &input_props));
    const TensorShapeProto& input_shape = input_props->shape();
    if (!PartialTensorShape(input_shape).IsFullyDefined()) {
      return Status::OK();
    }
    Tensor multiples(DT_INT32, TensorShape({input_shape.dim_size()}));
    TF_RETURN_IF_ERROR(CalculateMultiplesFromChain(chain, &multiples));

    const OpInfo::TensorProperties* output_props;
    TF_RETURN_IF_ERROR(GetTensorProperties(node->name(), &output_props));
    const TensorShapeProto& output_shape = output_props->shape();
    if (!PartialTensorShape(output_shape).IsFullyDefined()) {
      return Status::OK();
    }
    Tensor output_shape_tensor(DT_INT32,
                               TensorShape({output_shape.dim_size()}));
    for (int i = 0; i < output_shape.dim_size(); ++i) {
      output_shape_tensor.flat<int32>()(i) = output_shape.dim(i).size();
    }

    // 3. Create constant node with correct multiples value
    NodeDef* new_const_node = AddEmptyNode(new_const_name);
    TF_RETURN_IF_ERROR(ConstantFolding::CreateNodeDef(
        new_const_node->name(), TensorValue(&multiples), new_const_node));
    new_const_node->set_device(node->device());
    MaybeAddControlInput(input->name(), new_const_node, ctx().optimized_graph,
                         ctx().node_map);
    AddToOptimizationQueue(new_const_node);

    // 4. Replace the Pack node with Tile(Const(N), input);
    DataType dtype = GetDataTypeFromAttr(*node, "T");
    NodeDef* new_tile_node = AddEmptyNode(new_tile_name);
    new_tile_node->set_op("Tile");
    new_tile_node->set_device(node->device());
    SetDataTypeToAttr(dtype, "T", new_tile_node);
    SetDataTypeToAttr(DT_INT32, "Tmultiples", new_tile_node);
    new_tile_node->add_input(input->name());
    ctx().node_map->AddOutput(input->name(), new_tile_node->name());
    new_tile_node->add_input(new_const_node->name());
    ctx().node_map->AddOutput(new_const_node->name(), new_tile_node->name());

    // Tile inherits all control dependencies from the original pack chain
    ForwardControlDependencies(new_tile_node, chain);
    AddToOptimizationQueue(new_tile_node);

    // 5. Add a new Reshape node to preserve the existing shape
    NodeDef* new_shape_node = AddEmptyNode(new_shape_name);
    TF_RETURN_IF_ERROR(ConstantFolding::CreateNodeDef(
        new_shape_node->name(), TensorValue(&output_shape_tensor),
        new_shape_node));
    new_shape_node->set_device(node->device());
    MaybeAddControlInput(input->name(), new_shape_node, ctx().optimized_graph,
                         ctx().node_map);
    AddToOptimizationQueue(new_shape_node);

    NodeDef* new_reshape_node = AddEmptyNode(new_reshape_name);
    new_reshape_node->set_op("Reshape");
    new_reshape_node->set_device(node->device());
    SetDataTypeToAttr(dtype, "T", new_reshape_node);
    SetDataTypeToAttr(DT_INT32, "Tshape", new_reshape_node);
    new_reshape_node->add_input(new_tile_node->name());
    ctx().node_map->AddOutput(new_tile_node->name(), new_reshape_node->name());
    new_reshape_node->add_input(new_shape_node->name());
    ctx().node_map->AddOutput(new_shape_node->name(), new_reshape_node->name());

    *simplified_node_name = new_reshape_node->name();

    return Status::OK();
  }