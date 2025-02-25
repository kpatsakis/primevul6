void TFE_DeleteContextCapsule(PyObject* context) {
  TFE_Context* ctx =
      reinterpret_cast<TFE_Context*>(PyCapsule_GetPointer(context, nullptr));
  auto op = ReleaseThreadLocalOp(ctx);
  op.reset();
  TFE_DeleteContext(ctx);
}