  Local<Array> CreateHeaders() {
    // num_values_ is either -1 or the entry # of the last header
    // so num_values_ == 0 means there's a single header
    Local<Array> headers = Array::New(2 * num_values_);

    for (int i = 0; i < num_values_; ++i) {
      headers->Set(2 * i, fields_[i].ToString());
      headers->Set(2 * i + 1, values_[i].ToString());
    }

    return headers;
  }