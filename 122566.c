  HTTP_DATA_CB(on_header_value) {
    if (num_values_ != num_fields_) {
      // start of new header value
      num_values_++;
      values_[num_values_ - 1].Reset();
    }

    assert(num_values_ < (int)ARRAY_SIZE(values_));
    assert(num_values_ == num_fields_);

    values_[num_values_ - 1].Update(at, length);

    return 0;
  }