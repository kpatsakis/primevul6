  Status SetElementToOne(int i, Tensor* t) {
    switch (t->dtype()) {
      case DT_INT32:
        t->flat<int32>()(i) = 1;
        return Status::OK();
      case DT_INT64:
        t->flat<int64>()(i) = 1L;
        return Status::OK();
      case DT_FLOAT:
        t->flat<float>()(i) = 1.0f;
        return Status::OK();
      case DT_DOUBLE:
        t->flat<double>()(i) = 1.0;
        return Status::OK();
      case DT_COMPLEX64:
        t->flat<complex64>()(i) = complex64(1);
        return Status::OK();
      case DT_COMPLEX128:
        t->flat<complex128>()(i) = complex128(1);
        return Status::OK();
      default:
        return errors::InvalidArgument("Invalid data type: ", t->dtype());
    }
  }