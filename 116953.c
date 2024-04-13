GetAllAttrToDefaultsMaps() {
  static auto* all_attr_to_defaults_maps = new tensorflow::gtl::FlatMap<
      string, tensorflow::gtl::FlatMap<string, tensorflow::DataType>*>;
  return all_attr_to_defaults_maps;
}