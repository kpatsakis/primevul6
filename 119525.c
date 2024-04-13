  Status FindCommonUnaryOpChain(const NodeDef& root_node, int* prefix_length,
                                ChainLinkSet* tails,
                                std::set<string>* ctrl_inputs) const {
    *prefix_length = 0;
    // Follow the chains starting at each concat input or split output as long
    // as all the following conditions hold:
    //   1. The ops in all chains are the same.
    //   2. The ops are unary elementwise op.
    //   3. The op output has only a single consumer (concat only).
    ChainLinkSet cur_tails;
    TF_RETURN_IF_ERROR(InitializeChains(root_node, &cur_tails));
    if (cur_tails.size() < 2) {
      return Status::OK();
    }
    ctrl_inputs->clear();
    bool stop = false;
    while (!stop && !cur_tails.empty() &&
           OpsAreSafeToHoist(root_node, cur_tails)) {
      // We found one more link that can be hoisted.
      ++(*prefix_length);
      tails->swap(cur_tails);
      GatherControlInputs(ctrl_inputs, *tails);

      // Advance tail pointers to the next level.
      TF_RETURN_IF_ERROR(AdvanceTails(*tails, &cur_tails, &stop));
    }
    return Status::OK();
  }