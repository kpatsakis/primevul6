  void Compute(OpKernelContext* context) override {
    const Tensor& diagonal = context->input(0);

    // MatrixDiag and MatrixDiagV2 both use this OpKernel. MatrixDiag only has
    // one input, so we have to check the number of inputs before reading
    // additional parameters in MatrixDiagV2.
    int32_t lower_diag_index = 0;
    int32_t upper_diag_index = 0;
    int32_t num_rows = -1;
    int32_t num_cols = -1;
    T padding_value(0);

    // MatrixDiagOpV2-specific.
    if (context->num_inputs() > kNumV1Inputs) {
      auto& diag_index = context->input(1);
      OP_REQUIRES(context,
                  TensorShapeUtils::IsScalar(diag_index.shape()) ||
                      TensorShapeUtils::IsVector(diag_index.shape()),
                  errors::InvalidArgument(
                      "diag_index must be a scalar or vector, received shape: ",
                      diag_index.shape().DebugString()));
      OP_REQUIRES(context, diag_index.NumElements() > 0,
                  errors::InvalidArgument(
                      "Expected diag_index to have at least 1 element"));
      lower_diag_index = diag_index.flat<int32>()(0);
      upper_diag_index = lower_diag_index;
      if (TensorShapeUtils::IsVector(diag_index.shape())) {
        auto diag_index_size = diag_index.dim_size(0);
        OP_REQUIRES(
            context, 0 < diag_index_size && diag_index_size <= 2,
            errors::InvalidArgument(
                "diag_index must have only one or two elements, received ",
                diag_index_size, " elements."));
        if (diag_index_size > 1) {
          upper_diag_index = diag_index.flat<int32>()(1);
        }
      }

      auto& num_rows_tensor = context->input(2);
      OP_REQUIRES(context, TensorShapeUtils::IsScalar(num_rows_tensor.shape()),
                  errors::InvalidArgument("num_rows must be a scalar"));
      num_rows = num_rows_tensor.flat<int32>()(0);

      auto& num_cols_tensor = context->input(3);
      OP_REQUIRES(context, TensorShapeUtils::IsScalar(num_cols_tensor.shape()),
                  errors::InvalidArgument("num_cols must be a scalar"));
      num_cols = num_cols_tensor.flat<int32>()(0);

      auto& padding_value_tensor = context->input(4);
      OP_REQUIRES(context,
                  TensorShapeUtils::IsScalar(padding_value_tensor.shape()),
                  errors::InvalidArgument("padding_value must be a scalar"));
      padding_value = padding_value_tensor.flat<T>()(0);
    }

    // Size validations.
    const TensorShape& diagonal_shape = diagonal.shape();
    const int diag_rank = diagonal_shape.dims();
    const Eigen::Index num_diags = upper_diag_index - lower_diag_index + 1;
    OP_REQUIRES(context, TensorShapeUtils::IsVectorOrHigher(diagonal_shape),
                errors::InvalidArgument(
                    "diagonal must be at least 1-dim, received shape: ",
                    diagonal.shape().DebugString()));
    OP_REQUIRES(
        context, lower_diag_index <= upper_diag_index,
        errors::InvalidArgument(
            "lower_diag_index must not be larger than upper_diag_index: ",
            lower_diag_index, " > ", upper_diag_index));
    OP_REQUIRES(context,
                lower_diag_index == upper_diag_index ||
                    diagonal_shape.dim_size(diag_rank - 2) == num_diags,
                errors::InvalidArgument(
                    "The number of diagonals provided in the input does not "
                    "match the lower_diag_index and upper_diag_index range."));

    const Eigen::Index max_diag_len = diagonal_shape.dim_size(diag_rank - 1);
    const int32_t min_num_rows = max_diag_len - std::min(upper_diag_index, 0);
    const int32_t min_num_cols = max_diag_len + std::max(lower_diag_index, 0);
    OP_REQUIRES(context, num_rows == -1 || num_rows >= min_num_rows,
                errors::InvalidArgument("The number of rows is too small."));
    OP_REQUIRES(context, num_cols == -1 || num_cols >= min_num_cols,
                errors::InvalidArgument("The number of columns is too small."));

    // If both num_rows and num_cols are unknown, assume that output is square.
    // Otherwise, use smallest possible values.
    if (num_rows == -1 && num_cols == -1) {
      num_rows = std::max(min_num_rows, min_num_cols);
      num_cols = num_rows;
    } else if (num_rows == -1) {
      num_rows = min_num_rows;
    } else if (num_cols == -1) {
      num_cols = min_num_cols;
    }
    OP_REQUIRES(context, num_rows == min_num_rows || num_cols == min_num_cols,
                errors::InvalidArgument(
                    "The number of rows or columns is not consistent with "
                    "the specified d_lower, d_upper, and diagonal."));

    TensorShape output_shape = diagonal_shape;
    if (num_diags == 1) {  // Output has rank `rank+1`.
      output_shape.set_dim(diag_rank - 1, num_rows);
      output_shape.AddDim(num_cols);
    } else {  // Output has rank `rank`.
      output_shape.set_dim(diag_rank - 2, num_rows);
      output_shape.set_dim(diag_rank - 1, num_cols);
    }

    Tensor* output = nullptr;
    OP_REQUIRES_OK(context, context->allocate_output(0, output_shape, &output));
    auto output_reshaped = output->flat_inner_dims<T, 3>();
    auto diag_reshaped = diagonal.flat<T>();
    functor::MatrixDiag<Device, T>::Compute(
        context, context->eigen_device<Device>(), diag_reshaped,
        output_reshaped, lower_diag_index, upper_diag_index, max_diag_len,
        padding_value, left_align_superdiagonal_, left_align_subdiagonal_);
  }