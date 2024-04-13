  void AddControlInputs(std::set<string>* new_ctrl_inputs,
                        NodeDef* node) const {
    for (int i = node->input_size() - 1; i >= 0; --i) {
      const string& existing_input = node->input(i);
      if (!IsControlInput(existing_input)) break;
      new_ctrl_inputs->erase(existing_input);
    }
    for (const string& new_input : *new_ctrl_inputs) {
      ctx().node_map->AddOutput(NodeName(new_input), node->name());
      node->add_input(new_input);
    }
  }