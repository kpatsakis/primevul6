TfLiteRegistration* Register_EMBEDDING_LOOKUP_SPARSE() {
  static TfLiteRegistration r = {nullptr, nullptr, Prepare, Eval};
  return &r;
}