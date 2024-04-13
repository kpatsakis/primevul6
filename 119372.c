  bool InputMatchesTargetShape(const NodeDef& reshape) {
    const OpInfo::TensorProperties* reshape_props;
    const OpInfo::TensorProperties* input_props;

    if (!GetTensorProperties(reshape.name(), &reshape_props).ok() ||
        !GetTensorProperties(reshape.input(0), &input_props).ok()) {
      return false;
    }

    return ShapesSymbolicallyEqual(input_props->shape(),
                                   reshape_props->shape());
  }