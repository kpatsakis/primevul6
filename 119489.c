  string ShapeSignature(const TensorShapeProto& shape) const {
    string signature = strings::StrCat("rank:", shape.dim_size(), ":dim");
    for (int i = 0; i < shape.dim_size(); ++i)
      strings::StrAppend(&signature, ":", shape.dim(i).size());
    return signature;
  }