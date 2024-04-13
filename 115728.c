bool HasDynamicTensor(const TfLiteContext& context,
                      const TfLiteIntArray* int_array) {
  return HasDynamicTensorImpl(context, TfLiteIntArrayView{int_array});
}