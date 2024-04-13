  Status AbsorbInputByOptimizedNodesGroup(const string& input,
                                          OptimizedNodesGroup* group) const {
    std::deque<const string*> input_tensors;
    input_tensors.push_front(&input);

    while (!input_tensors.empty()) {
      const string* input_tensor = input_tensors.front();
      input_tensors.pop_front();

      // Get a node for the input tensor.
      NodeDef* input_node;
      TF_RETURN_IF_ERROR(GetInputNode(*input_tensor, &input_node));

      if (IsAbsorbableByOptimizedNodesGroup(*group, *input_node)) {
        group->optimized_nodes.push_back(input_node);
        for (int i = input_node->input_size() - 1; i >= 0; --i) {
          const string& absorbed_node_input = input_node->input(i);
          // TODO(ezhulenev): support control inputs
          if (IsControlInput(absorbed_node_input)) continue;
          input_tensors.push_front(&absorbed_node_input);
        }
      } else {
        // If input node can't be absorbed, add it to OptimizedNodesGroup input.
        const OpInfo::TensorProperties* properties;
        TF_RETURN_IF_ERROR(GetTensorProperties(*input_tensor, &properties));
        group->inputs.emplace_back(*input_tensor, properties->shape());
      }
    }

    return Status::OK();
  }