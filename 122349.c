bool AreTensorProtosEqual(const TensorProto& lhs, const TensorProto& rhs) {
  Tensor lhs_t(lhs.dtype());
  bool success = lhs_t.FromProto(lhs);
  DCHECK(success);

  Tensor rhs_t(rhs.dtype());
  success = rhs_t.FromProto(rhs);
  DCHECK(success);

  TensorProto lhs_tp;
  lhs_t.AsProtoTensorContent(&lhs_tp);

  TensorProto rhs_tp;
  rhs_t.AsProtoTensorContent(&rhs_tp);

  return AreSerializedProtosEqual(lhs_tp, rhs_tp);
}