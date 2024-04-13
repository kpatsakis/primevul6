TfLiteStatus Subgraph::BytesRequired(TfLiteType type, const int* dims,
                                     size_t dims_size, size_t* bytes) {
  TF_LITE_ENSURE(&context_, bytes != nullptr);
  size_t count = 1;
  for (int k = 0; k < dims_size; k++) {
    size_t old_count = count;
    TF_LITE_ENSURE_MSG(
        &context_,
        MultiplyAndCheckOverflow(old_count, dims[k], &count) == kTfLiteOk,
        "BytesRequired number of elements overflowed.\n");
  }
  size_t type_size = 0;
  TF_LITE_ENSURE_OK(&context_, GetSizeOfType(&context_, type, &type_size));
  TF_LITE_ENSURE_MSG(
      &context_, MultiplyAndCheckOverflow(type_size, count, bytes) == kTfLiteOk,
      "BytesRequired number of bytes overflowed.\n");
  return kTfLiteOk;
}