void LaunchConv2DBackpropFilterOp<Eigen::GpuDevice, T>::operator()(
    OpKernelContext* ctx, bool use_cudnn, bool cudnn_use_autotune,
    const Tensor& out_backprop, const Tensor& input, int row_dilation,
    int col_dilation, int row_stride, int col_stride, const Padding& padding,
    const std::vector<int64>& explicit_paddings, Tensor* filter_backprop,
    TensorFormat data_format) {
  using se::dnn::AlgorithmConfig;
  using se::dnn::AlgorithmDesc;
  using se::dnn::ProfileResult;

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
               filter_shape, out_backprop.shape(), dilations, strides, padding,
               explicit_paddings, data_format, &dims));

  int64 padding_top = -1, padding_bottom = -1;
  int64 padding_left = -1, padding_right = -1;
  if (padding == EXPLICIT) {
    GetExplicitPaddingForDim(explicit_paddings, data_format, 'H', &padding_top,
                             &padding_bottom);
    GetExplicitPaddingForDim(explicit_paddings, data_format, 'W', &padding_left,
                             &padding_right);
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

  auto* stream = ctx->op_device_context()->stream();
  OP_REQUIRES(ctx, stream, errors::Internal("No GPU stream available."));

  if (!use_cudnn) {
    ctx->SetStatus(errors::Unimplemented(
        "Conv2DBackprop for GPU is not currently supported "
        "without cudnn"));
    return;
  }

  // If the filter in-depth (filter_shape.dim_size(2)) is 1 and smaller than the
  // input depth, it's a depthwise convolution. More generally, if the filter
  // in-depth divides but is smaller than the input depth, it is a grouped
  // convolution.
  bool is_grouped_convolution = filter_shape.dim_size(2) != dims.in_depth;
  bool cudnn_disable_conv_1x1_optimization_ = CudnnDisableConv1x1Optimization();
  if (!cudnn_disable_conv_1x1_optimization_ &&
      dims.spatial_dims[0].filter_size == 1 &&
      dims.spatial_dims[1].filter_size == 1 && !is_grouped_convolution &&
      dims.spatial_dims[0].stride == 1 && dims.spatial_dims[1].stride == 1 &&
      data_format == FORMAT_NHWC && (padding == VALID || padding == SAME)) {
    const uint64 m = dims.in_depth;
    const uint64 k = dims.batch_size * dims.spatial_dims[0].input_size *
                     dims.spatial_dims[1].input_size;
    const uint64 n = dims.out_depth;

    // The shape of output backprop is
    //   [batch, out_rows, out_cols, out_depth]
    //   From cublas's perspective, it is: n x k
    auto a_ptr = AsDeviceMemory(out_backprop.template flat<T>().data(),
                                out_backprop.template flat<T>().size());

    // The shape of input is
    //   [batch, in_rows, in_cols, in_depth],
    //   From cublas's perspective, it is: m x k
    auto b_ptr = AsDeviceMemory(input.template flat<T>().data(),
                                input.template flat<T>().size());

    // the shape of the filter backprop from the conv_2d should be
    //   [1, 1, in_depth, out_depth]
    //   From cublas's perspective, it is: n x m
    auto c_ptr = AsDeviceMemory(filter_backprop->template flat<T>().data(),
                                filter_backprop->template flat<T>().size());

    bool blas_launch_status =
        stream
            ->ThenBlasGemm(se::blas::Transpose::kNoTranspose,
                           se::blas::Transpose::kTranspose, n, m, k, 1.0f,
                           a_ptr, n, b_ptr, m, 0.0f, &c_ptr, n)
            .ok();
    if (!blas_launch_status) {
      ctx->SetStatus(errors::Internal("Blas SGEMM launch failed : m=", m,
                                      ", n=", n, ", k=", k));
    }
    return;
  } else if (dims.spatial_dims[0].filter_size ==
                 dims.spatial_dims[0].input_size &&
             dims.spatial_dims[1].filter_size ==
                 dims.spatial_dims[1].input_size &&
             !is_grouped_convolution && padding == VALID &&
             data_format == FORMAT_NHWC) {
    // The input data and filter have the same height/width, and we are not
    // using grouped convolution, so call cublas directly.
    const uint64 m = dims.spatial_dims[0].input_size *
                     dims.spatial_dims[1].input_size * dims.in_depth;
    const uint64 k = dims.batch_size;
    const uint64 n = dims.out_depth;

    auto a_ptr = AsDeviceMemory(input.template flat<T>().data(),
                                input.template flat<T>().size());
    auto b_ptr = AsDeviceMemory(out_backprop.template flat<T>().data(),
                                out_backprop.template flat<T>().size());
    auto c_ptr = AsDeviceMemory(filter_backprop->template flat<T>().data(),
                                filter_backprop->template flat<T>().size());

    bool blas_launch_status =
        stream
            ->ThenBlasGemm(se::blas::Transpose::kNoTranspose,
                           se::blas::Transpose::kTranspose, n, m, k, 1.0f,
                           b_ptr, n, a_ptr, m, 0.0f, &c_ptr, n)
            .ok();
    if (!blas_launch_status) {
      ctx->SetStatus(errors::Internal("Blas SGEMM launch failed : m=", m,
                                      ", n=", n, ", k=", k));
    }
    return;
  }

  const int64 common_padding_rows = std::min(padding_top, padding_bottom);
  const int64 common_padding_cols = std::min(padding_left, padding_right);
  Tensor compatible_input;
  if (padding_top != padding_bottom || padding_left != padding_right) {
    // Pad the input in the same way we did during the forward pass, so that
    // cuDNN or MIOpen receives the same input during the backward pass function
    // as it did during the forward pass function.
    const int64 padding_rows_diff = std::abs(padding_bottom - padding_top);
    const int64 padding_cols_diff = std::abs(padding_right - padding_left);
    const int64 new_in_rows =
        dims.spatial_dims[0].input_size + padding_rows_diff;
    const int64 new_in_cols =
        dims.spatial_dims[1].input_size + padding_cols_diff;
    const int64 input_pad_top = padding_top - common_padding_rows;
    const int64 input_pad_bottom = padding_bottom - common_padding_rows;
    const int64 input_pad_left = padding_left - common_padding_cols;
    const int64 input_pad_right = padding_right - common_padding_cols;
    OP_REQUIRES_OK(
        ctx, ctx->allocate_temp(
                 DataTypeToEnum<T>::value,
                 ShapeFromFormat(data_format, dims.batch_size, new_in_rows,
                                 new_in_cols, dims.in_depth),
                 &compatible_input));

    functor::PadInput<GPUDevice, T, int, 4>()(
        ctx->template eigen_device<GPUDevice>(), To32Bit(input.tensor<T, 4>()),
        {{static_cast<int>(input_pad_top), static_cast<int>(input_pad_left)}},
        {{static_cast<int>(input_pad_bottom),
          static_cast<int>(input_pad_right)}},
        To32Bit(compatible_input.tensor<T, 4>()), data_format, T{});
  } else {
    compatible_input = input;
  }

  CHECK(common_padding_rows >= 0 && common_padding_cols >= 0)  // Crash OK
      << "Negative row or col paddings: (" << common_padding_rows << ", "
      << common_padding_cols << ")";

  // The Tensor Core in NVIDIA Volta+ GPUs supports efficient convolution with
  // fp16 in NHWC data layout. In all other configurations it's more efficient
  // to run computation in NCHW data format.
  const bool compute_in_nhwc =
      DataTypeToEnum<T>::value == DT_HALF && IsVoltaOrLater(*stream->parent());

  // We only do one directional conversion: NHWC->NCHW. We never convert in the
  // other direction. Grappler layout optimizer selects the preferred layout and
  // adds necessary annotations to the graph.
  const TensorFormat compute_data_format =
      (compute_in_nhwc && data_format == FORMAT_NHWC) ? FORMAT_NHWC
                                                      : FORMAT_NCHW;

  VLOG(3) << "Compute Conv2DBackpropFilter with cuDNN:"
          << " data_format=" << ToString(data_format)
          << " compute_data_format=" << ToString(compute_data_format);

  constexpr auto kComputeInNHWC =
      std::make_tuple(se::dnn::DataLayout::kBatchYXDepth,
                      se::dnn::FilterLayout::kOutputYXInput);
  constexpr auto kComputeInNCHW =
      std::make_tuple(se::dnn::DataLayout::kBatchDepthYX,
                      se::dnn::FilterLayout::kOutputInputYX);

  se::dnn::DataLayout compute_data_layout;
  se::dnn::FilterLayout filter_layout;

  std::tie(compute_data_layout, filter_layout) =
      compute_data_format == FORMAT_NHWC ? kComputeInNHWC : kComputeInNCHW;

  se::dnn::BatchDescriptor input_desc;
  input_desc.set_count(dims.batch_size)
      .set_height(GetTensorDim(compatible_input, data_format, 'H'))
      .set_width(GetTensorDim(compatible_input, data_format, 'W'))
      .set_feature_map_count(dims.in_depth)
      .set_layout(compute_data_layout);
  se::dnn::BatchDescriptor output_desc;
  output_desc.set_count(dims.batch_size)
      .set_height(dims.spatial_dims[0].output_size)
      .set_width(dims.spatial_dims[1].output_size)
      .set_feature_map_count(dims.out_depth)
      .set_layout(compute_data_layout);
  se::dnn::FilterDescriptor filter_desc;
  filter_desc.set_input_filter_height(dims.spatial_dims[0].filter_size)
      .set_input_filter_width(dims.spatial_dims[1].filter_size)
      .set_input_feature_map_count(filter_shape.dim_size(2))
      .set_output_feature_map_count(filter_shape.dim_size(3))
      .set_layout(filter_layout);
  se::dnn::ConvolutionDescriptor conv_desc;
  conv_desc.set_vertical_dilation_rate(dims.spatial_dims[0].dilation)
      .set_horizontal_dilation_rate(dims.spatial_dims[1].dilation)
      .set_vertical_filter_stride(dims.spatial_dims[0].stride)
      .set_horizontal_filter_stride(dims.spatial_dims[1].stride)
      .set_zero_padding_height(common_padding_rows)
      .set_zero_padding_width(common_padding_cols)
      .set_group_count(dims.in_depth / filter_shape.dim_size(2));

  // Tensorflow filter format: HWIO
  // cuDNN filter formats: (data format) -> (filter format)
  //   (1) NCHW -> OIHW
  //   (2) NHWC -> OHWI
  //
  // We compute filter backprop into temporary tensor, and then convert it to
  // the HWIO data format at the end.

  Tensor pre_transformed_filter_backprop;
  OP_REQUIRES_OK(
      ctx,
      ctx->allocate_temp(
          DataTypeToEnum<T>::value,
          TensorShape({filter_shape.dim_size(3), filter_shape.dim_size(2),
                       filter_shape.dim_size(0), filter_shape.dim_size(1)}),
          &pre_transformed_filter_backprop));

  Tensor transformed_out_backprop;
  if (data_format == FORMAT_NHWC && compute_data_format == FORMAT_NCHW) {
    VLOG(4) << "Convert the `out_backprop` tensor from NHWC to NCHW.";
    TensorShape compute_shape = ShapeFromFormat(
        compute_data_format, dims.batch_size, dims.spatial_dims[0].output_size,
        dims.spatial_dims[1].output_size, dims.out_depth);
    if (dims.out_depth > 1) {
      OP_REQUIRES_OK(ctx,
                     ctx->allocate_temp(DataTypeToEnum<T>::value, compute_shape,
                                        &transformed_out_backprop));
      functor::NHWCToNCHW<GPUDevice, T, 4>()(
          ctx->eigen_device<GPUDevice>(), out_backprop.tensor<T, 4>(),
          transformed_out_backprop.tensor<T, 4>());
    } else {
      // If depth <= 1, just reshape.
      CHECK(transformed_out_backprop.CopyFrom(out_backprop, compute_shape));
    }
  } else {
    transformed_out_backprop = out_backprop;
  }

  Tensor transformed_input;
  if (data_format == FORMAT_NHWC && compute_data_format == FORMAT_NCHW) {
    VLOG(4) << "Convert the `input` tensor from NHWC to NCHW.";
    TensorShape compute_shape = ShapeFromFormat(
        compute_data_format, GetTensorDim(compatible_input, data_format, 'N'),
        GetTensorDim(compatible_input, data_format, 'H'),
        GetTensorDim(compatible_input, data_format, 'W'),
        GetTensorDim(compatible_input, data_format, 'C'));
    if (compute_shape.dim_size(1) > 1) {
      OP_REQUIRES_OK(ctx,
                     ctx->allocate_temp(DataTypeToEnum<T>::value, compute_shape,
                                        &transformed_input));
      functor::NHWCToNCHW<GPUDevice, T, 4>()(
          ctx->eigen_device<GPUDevice>(),
          const_cast<const Tensor&>(compatible_input).tensor<T, 4>(),
          transformed_input.tensor<T, 4>());
    } else {
      // If depth <= 1, just reshape.
      CHECK(transformed_input.CopyFrom(compatible_input, compute_shape));
    }
  } else {
    transformed_input = compatible_input;
  }

  se::DeviceMemory<T> out_backprop_ptr =
      AsDeviceMemory(transformed_out_backprop.template flat<T>().data(),
                     transformed_out_backprop.template flat<T>().size());
  se::DeviceMemory<T> filter_backprop_ptr =
      AsDeviceMemory(pre_transformed_filter_backprop.template flat<T>().data(),
                     pre_transformed_filter_backprop.template flat<T>().size());
  auto input_ptr = AsDeviceMemory(transformed_input.template flat<T>().data(),
                                  transformed_input.template flat<T>().size());

  static int64 ConvolveBackwardFilterScratchSize = GetDnnWorkspaceLimit(
      "TF_CUDNN_WORKSPACE_LIMIT_IN_MB", 1LL << 32  // 4GB by default
  );
  int device_id = stream->parent()->device_ordinal();
  DataType dtype = input.dtype();
  ConvParameters conv_parameters = {
      dims.batch_size,                     // batch
      dims.in_depth,                       // in_depths
      {{input_desc.height(),               // in_rows
        input_desc.width()}},              // in_cols
      compute_data_format,                 // compute_data_format
      dims.out_depth,                      // out_depths
      {{dims.spatial_dims[0].filter_size,  // filter_rows
        dims.spatial_dims[1].filter_size,  // filter_cols
        filter_shape.dim_size(2)}},        // filter_depth
      {{dims.spatial_dims[0].dilation,     // dilation_rows
        dims.spatial_dims[1].dilation}},   // dilation_cols
      {{dims.spatial_dims[0].stride,       // stride_rows
        dims.spatial_dims[1].stride}},     // stride_cols
      {{common_padding_rows,               // padding_rows
        common_padding_cols}},             // padding_cols
      dtype,                               // tensor datatype
      device_id,                           // device_id
      conv_desc.group_count()              // group_count
  };
#if TENSORFLOW_USE_ROCM
  // cudnn_use_autotune is applicable only the CUDA flow
  // for ROCm/MIOpen, we need to call GetMIOpenConvolveAlgorithms explicitly
  // if we do not have a cached algorithm_config for this conv_parameters
  cudnn_use_autotune = true;
#endif
  AlgorithmConfig algorithm_config;

  if (cudnn_use_autotune && !AutoTuneConvBwdFilter::GetInstance()->Find(
                                conv_parameters, &algorithm_config)) {
    std::vector<std::unique_ptr<se::dnn::ConvolveExecutionPlan>> plans;
#if GOOGLE_CUDA
    std::vector<AlgorithmDesc> algorithms;
    std::vector<AlgorithmConfig> configs;

    if (CudnnUseFrontend()) {
      OP_REQUIRES(
          ctx,
          stream->parent()->GetConvolveExecutionPlans(
              se::dnn::ConvolutionKind::BACKWARD_FILTER,
              se::dnn::ToDataType<T>::value, stream, input_desc, filter_desc,
              output_desc, conv_desc, &plans),
          errors::Unknown("Failed to get convolution execution plan. This is "
                          "probably because cuDNN failed to initialize, so try "
                          "looking to see if a warning log message was printed "
                          "above."));
      for (const auto& plan : plans) {
        configs.push_back(
            AlgorithmConfig(AlgorithmDesc{plan->getTag(), plan->get_raw_desc()},
                            plan->getWorkspaceSize()));
      }
    } else {
      OP_REQUIRES(
          ctx,
          stream->parent()->GetConvolveBackwardFilterAlgorithms(
              conv_parameters.ShouldIncludeWinogradNonfusedAlgo<T>(
                  stream->parent()),
              &algorithms),
          errors::Unknown("Failed to get convolution execution plan. This is "
                          "probably because cuDNN failed to initialize, so try "
                          "looking to see if a warning log message was printed "
                          "above."));
      for (const auto& algorithm : algorithms) {
        configs.push_back(AlgorithmConfig(algorithm));
      }
    }

    se::TfAllocatorAdapter tf_allocator_adapter(ctx->device()->GetAllocator({}),
                                                stream);
    se::RedzoneAllocator rz_allocator(stream, &tf_allocator_adapter,
                                      se::GpuAsmOpts());

    se::DeviceMemory<T> filter_backprop_ptr_rz(
        WrapRedzoneBestEffort(&rz_allocator, filter_backprop_ptr));

    std::vector<tensorflow::AutotuneResult> results;
    for (auto& profile_config : configs) {
      // TODO(zhengxq): profile each algorithm multiple times to better
      // accuracy.
      DnnScratchAllocator scratch_allocator(ConvolveBackwardFilterScratchSize,
                                            ctx);
      se::RedzoneAllocator rz_scratch_allocator(
          stream, &tf_allocator_adapter, se::GpuAsmOpts(),
          /*memory_limit=*/ConvolveBackwardFilterScratchSize);
      se::ScratchAllocator* allocator_used =
          !RedzoneCheckDisabled()
              ? static_cast<se::ScratchAllocator*>(&rz_scratch_allocator)
              : static_cast<se::ScratchAllocator*>(&scratch_allocator);

      ProfileResult profile_result;

      Status cudnn_launch_status;
      if (CudnnUseFrontend()) {
        cudnn_launch_status = stream->ConvolveBackwardFilterWithExecutionPlan(
            input_desc, input_ptr, output_desc, out_backprop_ptr, conv_desc,
            filter_desc, &filter_backprop_ptr_rz, allocator_used,
            profile_config, &profile_result);
      } else {
        cudnn_launch_status = stream->ConvolveBackwardFilterWithAlgorithm(
            input_desc, input_ptr, output_desc, out_backprop_ptr, conv_desc,
            filter_desc, &filter_backprop_ptr_rz, allocator_used,
            profile_config, &profile_result);
      }
      if (cudnn_launch_status.ok() && profile_result.is_valid()) {
        results.emplace_back();
        auto& result = results.back();
        if (CudnnUseFrontend()) {
          result.mutable_cuda_conv_plan()->set_exec_plan_id(
              profile_config.algorithm()->exec_plan_id());
        } else {
          result.mutable_conv()->set_algorithm(
              profile_config.algorithm()->algo_id());
          result.mutable_conv()->set_tensor_ops_enabled(
              profile_config.algorithm()->tensor_ops_enabled());
        }

        result.set_scratch_bytes(
            !RedzoneCheckDisabled()
                ? rz_scratch_allocator.TotalAllocatedBytesExcludingRedzones()
                : scratch_allocator.TotalByteSize());
        *result.mutable_run_time() = proto_utils::ToDurationProto(
            absl::Milliseconds(profile_result.elapsed_time_in_ms()));

        CheckRedzones(rz_scratch_allocator, &result);
        CheckRedzones(rz_allocator, &result);
      } else if (CudnnUseFrontend()) {
        // When CuDNN frontend APIs are used, we need to make sure the profiling
        // results are one-to-one mapping of the "plans". So, we insert dummy
        // results when the excution fails.
        results.emplace_back();
        auto& result = results.back();
        result.mutable_failure()->set_kind(AutotuneResult::UNKNOWN);
        result.mutable_failure()->set_msg(
            absl::StrCat("Profiling failure on CUDNN engine: ",
                         profile_config.algorithm()->exec_plan_id()));
      }
    }
#elif TENSORFLOW_USE_ROCM
    DnnScratchAllocator scratch_allocator(ConvolveBackwardFilterScratchSize,
                                          ctx);

    std::vector<ProfileResult> algorithms;
    OP_REQUIRES(
        ctx,
        stream->parent()->GetMIOpenConvolveAlgorithms(
            se::dnn::ConvolutionKind::BACKWARD_FILTER,
            se::dnn::ToDataType<T>::value, stream, input_desc, input_ptr,
            filter_desc, filter_backprop_ptr, output_desc, out_backprop_ptr,
            conv_desc, &scratch_allocator, &algorithms),
        errors::Unknown(
            "Failed to get convolution algorithm. This is probably "
            "because MIOpen failed to initialize, so try looking to "
            "see if a warning log message was printed above."));

    std::vector<tensorflow::AutotuneResult> results;
    if (algorithms.size() == 1) {
      auto profile_result = algorithms[0];
      results.emplace_back();
      auto& result = results.back();
      result.mutable_conv()->set_algorithm(
          profile_result.algorithm().algo_id());
      result.mutable_conv()->set_tensor_ops_enabled(
          profile_result.algorithm().tensor_ops_enabled());

      result.set_scratch_bytes(profile_result.scratch_size());
      *result.mutable_run_time() = proto_utils::ToDurationProto(
          absl::Milliseconds(profile_result.elapsed_time_in_ms()));
    } else {
      for (auto miopen_algorithm : algorithms) {
        auto profile_algorithm = miopen_algorithm.algorithm();
        ProfileResult profile_result;
        auto miopen_launch_status = stream->ConvolveBackwardFilterWithAlgorithm(
            input_desc, input_ptr, output_desc, out_backprop_ptr, conv_desc,
            filter_desc, &filter_backprop_ptr, &scratch_allocator,
            AlgorithmConfig(profile_algorithm, miopen_algorithm.scratch_size()),
            &profile_result);

        if (miopen_launch_status.ok() && profile_result.is_valid()) {
          results.emplace_back();
          auto& result = results.back();
          result.mutable_conv()->set_algorithm(profile_algorithm.algo_id());
          result.mutable_conv()->set_tensor_ops_enabled(
              profile_algorithm.tensor_ops_enabled());
          result.set_scratch_bytes(scratch_allocator.TotalByteSize());
          *result.mutable_run_time() = proto_utils::ToDurationProto(
              absl::Milliseconds(profile_result.elapsed_time_in_ms()));
        }
      }
    }
#endif
    LogConvAutotuneResults(se::dnn::ConvolutionKind::BACKWARD_FILTER,
                           se::dnn::ToDataType<T>::value, input_ptr,
                           filter_backprop_ptr, out_backprop_ptr, input_desc,
                           filter_desc, output_desc, conv_desc,
                           stream->parent(), results);
    if (CudnnUseFrontend()) {
      OP_REQUIRES_OK(
          ctx, BestCudnnConvAlgorithm(results, &plans, &algorithm_config));
    } else {
      OP_REQUIRES_OK(
          ctx, BestCudnnConvAlgorithm(results, nullptr, &algorithm_config));
    }
    AutoTuneConvBwdFilter::GetInstance()->Insert(conv_parameters,
                                                 algorithm_config);
  }

  Status cudnn_launch_status;
  DnnScratchAllocator scratch_allocator(ConvolveBackwardFilterScratchSize, ctx);
  if (CudnnUseFrontend()) {
    if (algorithm_config.algorithm().has_value()) {
      VLOG(4) << "Conv2DBackpropFilter Execution Plan: "
              << algorithm_config.algorithm()->exec_plan_id();
    } else {
      VLOG(4) << "Convolution AutoTune has been turned off";
    }
    cudnn_launch_status = stream->ConvolveBackwardFilterWithExecutionPlan(
        input_desc, input_ptr, output_desc, out_backprop_ptr, conv_desc,
        filter_desc, &filter_backprop_ptr, &scratch_allocator, algorithm_config,
        nullptr);
  } else {
    cudnn_launch_status = stream->ConvolveBackwardFilterWithAlgorithm(
        input_desc, input_ptr, output_desc, out_backprop_ptr, conv_desc,
        filter_desc, &filter_backprop_ptr, &scratch_allocator, algorithm_config,
        nullptr);
  }

  if (!cudnn_launch_status.ok()) {
    ctx->SetStatus(cudnn_launch_status);
    return;
  }

  FilterTensorFormat src_filter_format =
      compute_data_format == FORMAT_NCHW ? FORMAT_OIHW : FORMAT_OHWI;

  auto toConstTensor = [](const Tensor& x) -> const Tensor { return x; };
  functor::ReverseTransformFilter<GPUDevice, T, 4>()(
      ctx->eigen_device<GPUDevice>(), src_filter_format,
      toConstTensor(pre_transformed_filter_backprop).template tensor<T, 4>(),
      filter_backprop->tensor<T, 4>());
}