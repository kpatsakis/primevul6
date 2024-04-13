  Status InitializeChains(const NodeDef& node, ChainLinkSet* tails) const {
    if (node_is_concat_) {
      // Handle concat nodes by looking backwards in the graph.
      TF_RETURN_IF_ERROR(CheckAttrExists(node, "N"));
      const int n = node.attr().at("N").i();
      const int start = node.op() == "Concat" ? 1 : 0;
      const int end = start + n;
      if (end > node.input_size()) {
        return errors::FailedPrecondition("Got attr N=", n,
                                          " without enough inputs.");
      }
      // Set up tail pointers to point to the immediate inputs to Concat.
      for (int input_port = start; input_port < end; ++input_port) {
        if (IsControlInput(node.input(input_port))) {
          return errors::FailedPrecondition(
              "Got control input ", node.input(input_port),
              " where normal input was expected.");
        }
        NodeDef* tail;
        TF_RETURN_IF_ERROR(GetInputNode(node.input(input_port), &tail));
        tails->insert(ChainLink(tail, input_port));
      }
      return Status::OK();
    } else {
      // Handle split nodes by looking forwards in the graph.
      const auto& outputs = ctx().node_map->GetOutputs(node.name());
      for (NodeDef* output : outputs) {
        if (IsControlInput(output->input(0))) continue;
        TensorId tensor_id = ParseTensorName(output->input(0));
        if (tensor_id.node() == node.name()) {
          tails->insert(ChainLink(output, tensor_id.index()));
        } else {
          // This output node has a non-control input other than the split node,
          // abort.
          tails->clear();
          return Status::OK();
        }
      }
    }
    return Status::OK();
  }