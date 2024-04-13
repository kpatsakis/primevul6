Tensor CreateStringTensor(const string& value) {
  Tensor tensor(DT_STRING, TensorShape({}));
  tensor.scalar<tstring>()() = value;
  return tensor;
}