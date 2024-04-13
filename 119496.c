  void CopyTileWithInput(const NodeDef* tile, NodeDef* new_tile,
                         const string& input, const string& multiples) {
    new_tile->set_op("Tile");
    new_tile->set_device(tile->device());
    SetDataTypeToAttr(GetDataTypeFromAttr(*tile, "T"), "T", new_tile);
    SetDataTypeToAttr(GetDataTypeFromAttr(*tile, "Tmultiples"), "Tmultiples",
                      new_tile);

    new_tile->add_input(input);
    ctx().node_map->AddOutput(NodeName(input), new_tile->name());
    new_tile->add_input(multiples);
    ctx().node_map->AddOutput(NodeName(multiples), new_tile->name());

    AddToOptimizationQueue(new_tile);
  }