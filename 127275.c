  void operator()(OpKernelContext* ctx, bool use_cudnn, bool cudnn_use_autotune,
                  const Tensor& out_backprop, const Tensor& input,
                  int row_dilation, int col_dilation, int row_stride,
                  int col_stride, const Padding& padding,
                  const std::vector<int64>& explicit_paddings,
                  Tensor* filter_backprop, TensorFormat data_format) {
    std::vector<int32> dilations(4, 1);
    dilations[GetTensorDimIndex(data_format, 'H')] = row_dilation;
    dilations[GetTensorDimIndex(data_format, 'W')] = col_dilation;

    std::vector<int32> strides(4, 1);
    strides[GetTensorDimIndex(data_format, 'H')] = row_stride;
    strides[GetTensorDimIndex(data_format, 'W')] = col_stride;
    TensorShape filter_shape = filter_backprop->shape();

    ConvBackpropDimensions dims;
    OP_REQUIRES_OK(
        ctx, ConvBackpropComputeDimensionsV2(
                 "Conv2DBackpropFilter", /*num_spatial_dims=*/2, input.shape(),
                 filter_shape, out_backprop.shape(), dilations, strides,
                 padding, explicit_paddings, data_format, &dims));

    int64 padding_top = -1, padding_bottom = -1;
    int64 padding_left = -1, padding_right = -1;
    if (padding == EXPLICIT) {
      GetExplicitPaddingForDim(explicit_paddings, data_format, 'H',
                               &padding_top, &padding_bottom);
      GetExplicitPaddingForDim(explicit_paddings, data_format, 'W',
                               &padding_left, &padding_right);
    }
    int64 expected_out_rows, expected_out_cols;
    // The function is guaranteed to succeed because we checked the output and
    // padding was valid earlier.
    TF_CHECK_OK(GetWindowedOutputSizeVerboseV2(
        dims.spatial_dims[0].input_size, dims.spatial_dims[0].filter_size,
        row_dilation, row_stride, padding, &expected_out_rows, &padding_top,
        &padding_bottom));
    DCHECK_EQ(dims.spatial_dims[0].output_size, expected_out_rows);
    TF_CHECK_OK(GetWindowedOutputSizeVerboseV2(
        dims.spatial_dims[1].input_size, dims.spatial_dims[1].filter_size,
        col_dilation, col_stride, padding, &expected_out_cols, &padding_left,
        &padding_right));
    DCHECK_EQ(dims.spatial_dims[1].output_size, expected_out_cols);

    const CPUDevice& d = ctx->eigen_device<CPUDevice>();

    // WARNING: Need to swap row/col, padding_top/padding_left, and
    // padding_bottom/padding_right when calling Eigen. Eigen expects tensors
    // in NWHC format, but Tensorflow uses NHWC.

    auto filter_backprop_t = filter_backprop->tensor<T, 4>();
    auto input_t = input.tensor<T, 4>();
    auto out_backprop_t = out_backprop.tensor<T, 4>();

    if (padding != EXPLICIT) {
      // If padding was not explicitly defined, Eigen spatial convolution
      // backward filter will infer correct forward paddings from input tensors.
      filter_backprop_t.device(d) = Eigen::SpatialConvolutionBackwardKernel(
          input_t, out_backprop_t, filter_backprop_t.dimension(1),
          filter_backprop_t.dimension(0), col_stride, row_stride, col_dilation,
          row_dilation);

    } else {
      // Otherwise we have to explicitly pad the input, before passing it to
      // spatial convolution backward filter.
      Eigen::array<std::pair<int, int>, 4> paddings;
      paddings[0] = {0, 0};
      paddings[1] = {padding_top, padding_bottom};
      paddings[2] = {padding_left, padding_right};
      paddings[3] = {0, 0};

      auto padded_t = input_t.pad(paddings, T(0));

      // TODO(ezhulenev): Pass explicit paddings to Eigen spatial backward
      // convolution and do not rely on tensor padding expression.
      filter_backprop_t.device(d) = Eigen::SpatialConvolutionBackwardKernel(
          padded_t, out_backprop_t, filter_backprop_t.dimension(1),
          filter_backprop_t.dimension(0), col_stride, row_stride, col_dilation,
          row_dilation);
    }
  }