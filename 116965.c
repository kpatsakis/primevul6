  Status BuildOnesLike(const PyTapeTensor& t,
                       PyObject** result) const override {
    *result = t.OnesLike();
    return Status::OK();
  }