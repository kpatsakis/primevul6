  HTTP_DATA_CB(on_header_field) {
    if (num_fields_ == num_values_) {
      // start of new field name
      num_fields_++;
      if (num_fields_ == ARRAY_SIZE(fields_)) {
        // ran out of space - flush to javascript land
        Flush();
        num_fields_ = 1;
        num_values_ = 0;
      }
      fields_[num_fields_ - 1].Reset();
    }

    assert(num_fields_ < (int)ARRAY_SIZE(fields_));
    assert(num_fields_ == num_values_ + 1);

    fields_[num_fields_ - 1].Update(at, length);

    return 0;
  }