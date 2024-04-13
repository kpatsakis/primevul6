const string TensorSliceReader::DebugString() const {
  string shape_str;
  if (status().ok()) {
    for (const auto& e : Tensors()) {
      strings::StrAppend(&shape_str, e.first, " (",
                         DataType_Name(e.second->type()), ") ",
                         e.second->shape().DebugString());
      // Indicates if a tensor has more than 1 slice (i.e., it's partitioned).
      const int num_slices = e.second->Slices().size();
      if (num_slices > 1) {
        strings::StrAppend(&shape_str, ", ", num_slices, " slices");
      }
      strings::StrAppend(&shape_str, "\n");
    }
  }
  return shape_str;
}