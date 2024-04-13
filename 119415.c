  Status GetUniqueFactors(const NodeDef* node, const string& common_factor,
                          const bool common_factor_is_denominator,
                          bool* shapes_match,
                          std::vector<string>* unique_factors) const {
    *shapes_match = true;
    unique_factors->reserve(node->input_size());

    for (int i = 0; i < node->input_size() && *shapes_match; ++i) {
      const string& input = node->input(i);
      if (IsControlInput(input)) {
        break;
      }
      NodeDef* inner_node;
      TF_RETURN_IF_ERROR(GetInputNode(input, &inner_node));
      const int unique_factor_index =
          common_factor_is_denominator
              ? 0
              : (inner_node->input(0) == common_factor ? 1 : 0);
      unique_factors->push_back(inner_node->input(unique_factor_index));
      if (i > 0 && !IsAdd(*node)) {
        const OpInfo::TensorProperties* lhs;
        const OpInfo::TensorProperties* rhs;
        TF_RETURN_IF_ERROR(GetTensorProperties(unique_factors->front(), &lhs));
        TF_RETURN_IF_ERROR(GetTensorProperties(unique_factors->back(), &rhs));
        *shapes_match = ShapesSymbolicallyEqual(*lhs, *rhs);
      }
    }
    return Status::OK();
  }