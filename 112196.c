  Status ValidateInputTensor(const Tensor& tensor,
                             const std::string& tensor_name,
                             const Tensor& rhs) {
    const int ndims = rhs.dims();
    if (tensor.dims() != ndims) {
      return errors::InvalidArgument(tensor_name,
                                     " must have same rank as rhs, but got ",
                                     tensor.dims(), " and ", ndims);
    }
    for (int i = 0; i < ndims - 2; i++) {
      if (tensor.dim_size(i) != rhs.dim_size(i)) {
        return errors::InvalidArgument(
            tensor_name,
            " must have same outer dimensions as rhs, but for index ", i,
            ", got ", tensor.dim_size(i), " and ", rhs.dim_size(i));
      }
    }
    if (tensor.dim_size(ndims - 2) != 1) {
      return errors::InvalidArgument(
          tensor_name, "'s second-to-last dimension must be 1, but got ",
          tensor.dim_size(ndims - 2));
    }
    if (tensor.dim_size(ndims - 1) != rhs.dim_size(ndims - 2)) {
      return errors::InvalidArgument(tensor_name,
                                     "'s last dimension size must be rhs's "
                                     "second-to-last dimension size, but got ",
                                     tensor.dim_size(ndims - 1), " and ",
                                     rhs.dim_size(ndims - 2));
    }
    return Status::OK();
  }