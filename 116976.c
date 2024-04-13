AttrToInputsMap* GetAttrToInputsMapHoldingGIL(const tensorflow::OpDef& op_def) {
#if PY_MAJOR_VERSION >= 3 && PY_MINOR_VERSION >= 4
  DCHECK(PyGILState_Check())
      << "This function needs to hold the GIL when called.";
#endif
  auto* all_attr_to_input_maps = GetAllAttrToInputsMaps();
  auto* output =
      tensorflow::gtl::FindPtrOrNull(*all_attr_to_input_maps, op_def.name());
  if (output != nullptr) {
    return output;
  }

  std::unique_ptr<AttrToInputsMap> m(new AttrToInputsMap);

  // Store a list of InputIndex -> List of corresponding inputs.
  for (int i = 0; i < op_def.input_arg_size(); i++) {
    if (!op_def.input_arg(i).type_attr().empty()) {
      auto it = m->find(op_def.input_arg(i).type_attr());
      if (it == m->end()) {
        it = m->insert({op_def.input_arg(i).type_attr(), {}}).first;
      }
      it->second.emplace_back(i, !op_def.input_arg(i).number_attr().empty());
    }
  }

  auto* retval = m.get();
  (*all_attr_to_input_maps)[op_def.name()] = m.release();

  return retval;
}