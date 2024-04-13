  Status TrySimplify(NodeDef* reduction_node,
                     string* simplified_node_name) override {
    if (IsInPreserveSet(*reduction_node)) {
      return Status::OK();
    }

    NodeDef* inner_function;
    TF_RETURN_IF_ERROR(GetInputNode(reduction_node->input(0), &inner_function));

    NodeDef* inner_function_input = nullptr;
    if (inner_function->input_size() > 0) {
      TF_RETURN_IF_ERROR(
          GetInputNode(inner_function->input(0), &inner_function_input));
    }

    // Optimize only if:
    // 0. inner_function is not in the preserve set,
    // 1. inner_function's Op is element-wise monotonic
    // 2. inner_function's output is not being consumed elsewhere.
    // 3. is monotonic increasing if reduction_node is a pooling operation
    //    since we don't have MinPool operations.
    // 4. inner_functions is not a Relu node with an input from FusedBatchNorm
    //    or BiasAdd. This pattern will be fused later by remapper.
    auto can_be_fused_by_remapper = [](const NodeDef& consumer,
                                       const NodeDef& producer) -> bool {
      if (IsRelu(consumer) || IsRelu6(consumer)) {
        if (IsFusedBatchNorm(producer) || IsBiasAdd(producer)) {
          return true;
        }
      }
      return false;
    };
    bool is_non_decreasing = false;
    if (!IsInPreserveSet(*inner_function) &&
        IsElementWiseMonotonic(*inner_function, &is_non_decreasing) &&
        ctx().node_map->GetOutputs(inner_function->name()).size() == 1 &&
        (is_non_decreasing || !IsAnyMaxPool(*reduction_node)) &&
        !can_be_fused_by_remapper(*inner_function, *inner_function_input)) {
      // Swap the first inputs of the inner function Op & the reduction Op.
      NodeDef* inner_input;
      TF_RETURN_IF_ERROR(GetInputNode(inner_function->input(0), &inner_input));
      reduction_node->set_input(0, inner_input->name());
      ctx().node_map->UpdateInput(reduction_node->name(),
                                  inner_function->name(), inner_input->name());
      inner_function->set_input(0, reduction_node->name());
      UpdateConsumers(reduction_node, inner_function->name());
      ctx().node_map->UpdateInput(inner_function->name(), inner_input->name(),
                                  reduction_node->name());
      if (!is_non_decreasing) {
        // Flip Min<->Max if the function is non-increasing, e.g.
        // Max(Neg(x)) = Neg(Min(x)).
        const string opposite = FlipMinMax(*reduction_node);
        reduction_node->set_op(opposite);
      }

      if (IsArgMax(*reduction_node) || IsArgMin(*reduction_node)) {
        // ArgMax(Sqrt(x)) = ArgMax(x)
        inner_function->set_op("Identity");
      }

      AddToOptimizationQueue(reduction_node);
      AddToOptimizationQueue(inner_function);
      AddToOptimizationQueue(inner_input);
    }
    return Status::OK();
  }