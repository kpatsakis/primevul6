Status GetElementShapeFromInput(OpKernelContext* c,
                                const TensorList& tensor_list, int index,
                                PartialTensorShape* element_shape) {
  TF_RETURN_IF_ERROR(TensorShapeFromTensor(c->input(index), element_shape));
  // Check that `element_shape` and `tensor_list.element_shape` are
  // compatible and store the merged shape in `element_shape`.
  PartialTensorShape tmp = *element_shape;
  TF_RETURN_IF_ERROR(tmp.MergeWith(tensor_list.element_shape, element_shape));
  return Status::OK();
}