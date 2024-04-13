  void GatherControlInputs(std::set<string>* ctrl_inputs,
                           const ChainLinkSet& ops) const {
    for (const auto& link : ops) {
      const NodeDef* node = link.node;
      for (int i = node->input_size() - 1; i >= 0; --i) {
        const string& input = node->input(i);
        if (!IsControlInput(input)) break;
        ctrl_inputs->insert(input);
      }
    }
  }