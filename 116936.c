GetAttrToDefaultsMapHoldingGIL(const tensorflow::OpDef& op_def) {
#if PY_MAJOR_VERSION >= 3 && PY_MINOR_VERSION >= 4
  DCHECK(PyGILState_Check())
      << "This function needs to hold the GIL when called.";
#endif
  auto* all_attr_to_defaults_maps = GetAllAttrToDefaultsMaps();
  auto* output =
      tensorflow::gtl::FindPtrOrNull(*all_attr_to_defaults_maps, op_def.name());
  if (output != nullptr) {
    return output;
  }

  auto* new_map = new tensorflow::gtl::FlatMap<string, tensorflow::DataType>;

  for (const auto& attr : op_def.attr()) {
    if (attr.type() == "type" && attr.has_default_value()) {
      new_map->insert({attr.name(), attr.default_value().type()});
    }
  }

  (*all_attr_to_defaults_maps)[op_def.name()] = new_map;

  return new_map;
}