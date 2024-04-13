  Status SetInputType(DataType dtype, NodeDef* node) {
    const OpDef* op_def = nullptr;
    TF_RETURN_IF_ERROR(OpRegistry::Global()->LookUpOpDef(node->op(), &op_def));
    const OpDef::ArgDef& input_arg = op_def->input_arg(0);
    const string& type_attr_name = input_arg.type_attr();
    if (type_attr_name.empty()) {
      if (input_arg.type() == DT_INVALID || input_arg.type() != dtype) {
        return errors::InvalidArgument("Could not set input type of ",
                                       node->op(), " op to ",
                                       DataTypeString(dtype));
      } else {
        // Op has fixed input type that already matches dtype.
        return Status::OK();
      }
    }
    SetDataTypeToAttr(dtype, type_attr_name, node);
    return Status::OK();
  }