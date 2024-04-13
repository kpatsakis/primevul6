  Status AdvanceTails(const ChainLinkSet& tails, ChainLinkSet* new_tails,
                      bool* stop) const {
    *stop = true;
    new_tails->clear();
    for (const auto& link : tails) {
      const NodeDef* tail = link.node;
      if (node_is_concat_) {
        if (tail->input_size() == 0 || IsControlInput(tail->input(0))) {
          return Status::OK();
        }
        NodeDef* new_tail;
        TF_RETURN_IF_ERROR(GetInputNode(tail->input(0), &new_tail));
        // Remember original port.
        new_tails->insert(ChainLink(new_tail, link.port_origin));
      } else {
        for (NodeDef* new_tail : ctx().node_map->GetOutputs(tail->name())) {
          const TensorId tensor = ParseTensorName(new_tail->input(0));
          if (tensor.node() != tail->name()) {
            return Status::OK();
          }
          // Skip control outputs.
          if (tensor.index() >= 0) {
            // Remember original port.
            new_tails->insert(ChainLink(new_tail, link.port_origin));
          }
        }
      }
    }
    *stop = false;
    return Status::OK();
  }