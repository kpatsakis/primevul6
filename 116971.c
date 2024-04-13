TFE_Op* GetFunc(TFE_Context* ctx, const tensorflow::NameAttrList& func,
                TF_Status* status) {
  TFE_Op* func_op = TFE_NewOp(ctx, func.name().data(), status);
  for (const auto& attr : func.attr()) {
    if (!status->status.ok()) return nullptr;
    SetOpAttrValueScalar(ctx, func_op, attr.second, attr.first.data(), status);
    if (!status->status.ok()) return nullptr;
  }
  return func_op;
}