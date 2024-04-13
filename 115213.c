TfLiteRegistration* Register_SPLIT_V() {
  static TfLiteRegistration r = {nullptr, nullptr, split_v::Prepare,
                                 split_v::Eval};
  return &r;
}