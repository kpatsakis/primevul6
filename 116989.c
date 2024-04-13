tensorflow::gtl::FlatMap<string, AttrToInputsMap*>* GetAllAttrToInputsMaps() {
  static auto* all_attr_to_input_maps =
      new tensorflow::gtl::FlatMap<string, AttrToInputsMap*>;
  return all_attr_to_input_maps;
}