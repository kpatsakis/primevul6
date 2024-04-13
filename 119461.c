  Status GetCommonFactors(const NodeDef* node, std::set<string>* common_factors,
                          bool* common_factor_is_denominator,
                          std::vector<string>* ctrl_deps) const {
    CHECK(common_factors->empty());
    CHECK_NOTNULL(common_factor_is_denominator);
    *common_factor_is_denominator = false;

    bool has_mul = false;
    bool has_div = false;
    for (int i = 0; i < node->input_size(); ++i) {
      if (i > 0 && common_factors->empty()) break;
      if (IsControlInput(node->input(i))) {
        ctrl_deps->push_back(node->input(i));
        continue;
      }
      NodeDef* input;
      TF_RETURN_IF_ERROR(GetInputNode(node->input(i), &input));

      if ((!IsMul(*input) && !IsAnyDiv(*input)) || (IsMul(*input) && has_div) ||
          (IsAnyDiv(*input) && has_mul)) {
        // Break if input is neither a Mul or Div, or if there are both Mul &
        // Div Ops.
        common_factors->clear();
        break;
      } else if (IsAnyDiv(*input)) {
        has_div = true;
        // In case of possible common dividers, we avoid hoisting out if any
        // input is not float/double, since integer division is not distributive
        // over addition.
        const OpInfo::TensorProperties* properties0;
        const OpInfo::TensorProperties* properties1;
        TF_RETURN_IF_ERROR(GetTensorProperties(input->input(0), &properties0));
        TF_RETURN_IF_ERROR(GetTensorProperties(input->input(1), &properties1));
        if (properties0->dtype() != DT_FLOAT &&
            properties0->dtype() != DT_DOUBLE &&
            properties1->dtype() != DT_FLOAT &&
            properties1->dtype() != DT_DOUBLE) {
          common_factors->clear();
          break;
        }
      } else if (IsMul(*input)) {
        has_mul = true;
      }

      // We only focus on common factors from denominators if any Op is a
      // Div.
      std::set<string> factors_i =
          has_mul ? std::set<string>{input->input(0), input->input(1)}
                  : std::set<string>{input->input(1)};
      if (i == 0) {
        std::swap(*common_factors, factors_i);
      } else {
        std::set<string> intersection;
        std::set_intersection(
            factors_i.begin(), factors_i.end(), common_factors->begin(),
            common_factors->end(),
            std::inserter(intersection, intersection.begin()));
        std::swap(*common_factors, intersection);
      }
      for (int i = 2; i < input->input_size(); ++i) {
        ctrl_deps->push_back(input->input(i));
      }
    }

    *common_factor_is_denominator = has_div;
    return Status::OK();
  }