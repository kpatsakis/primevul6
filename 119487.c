NodeDef* GetTailOfIdempotentChain(
    const NodeDef& node, const NodeMap& node_map,
    const std::unordered_set<string>& nodes_to_preserve) {
  auto is_idempotent_non_branching = [&](const NodeDef& node) {
    return nodes_to_preserve.find(node.name()) == nodes_to_preserve.end() &&
           IsIdempotent(node) && NumNonControlOutputs(node, node_map) == 1;
  };
  return GetTailOfChain(node, node_map, /*follow_control_input=*/false,
                        is_idempotent_non_branching);
}