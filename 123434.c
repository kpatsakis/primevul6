Status GetInputList(OpKernelContext* c, int index, const TensorList** list) {
  if (!TensorShapeUtils::IsScalar(c->input(index).shape())) {
    return errors::InvalidArgument("Input list must be a scalar saw: ",
                                   c->input(index).shape().DebugString());
  }
  const TensorList* l = c->input(index).scalar<Variant>()().get<TensorList>();
  if (l == nullptr) {
    return errors::InvalidArgument(
        "Input handle is not a list. Saw: '",
        c->input(index).scalar<Variant>()().DebugString(), "'");
  }
  *list = l;
  return Status::OK();
}