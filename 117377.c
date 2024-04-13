  static void launch(OpKernelContext* context, const Tensor& tensor_in,
                     const Tensor& tensor_out, const Tensor& out_backprop,
                     const std::array<int64, 3>& window,
                     const std::array<int64, 3>& stride,
                     const std::array<int64, 3>& out,
                     const std::array<int64, 3>& padding,
                     TensorFormat data_format, Tensor* output) {
    output->flat<T>().setZero();
    for (int64 p = 0; p < out_backprop.dim_size(3); ++p) {
      // Calculate broadcast size for planes/rows/cols. For SAME padding,
      // current index could be in the padding area, and
      //   p * stride_planes + window_planes
      // could be beyond the input tensor's boundary. In such cases, change
      // the starting index and reduce the broadcast size.
      //
      // The same procedure is repeated for every spatial dimension in the
      // nested loops below.
      int pindex, psize;
      std::array<int64, 3> input_size{{tensor_in.dim_size(3),
                                       tensor_in.dim_size(2),
                                       tensor_in.dim_size(1)}};
      OP_REQUIRES_OK(context,
                     GetBroadcastSize(p, input_size[0], window[0], stride[0],
                                      padding[0], &pindex, &psize));
      for (int64 r = 0; r < out_backprop.dim_size(2); ++r) {
        int rindex, rsize;
        OP_REQUIRES_OK(context,
                       GetBroadcastSize(r, input_size[1], window[1], stride[1],
                                        padding[1], &rindex, &rsize));
        for (int64 c = 0; c < out_backprop.dim_size(1); ++c) {
          int cindex, csize;
          OP_REQUIRES_OK(
              context, GetBroadcastSize(c, input_size[2], window[2], stride[2],
                                        padding[2], &cindex, &csize));
          TensorSlice src{{0, -1}, {c, 1}, {r, 1}, {p, 1}, {0, -1}};
          TensorSlice dst{{0, -1},
                          {cindex, csize},
                          {rindex, rsize},
                          {pindex, psize},
                          {0, -1}};
          Eigen::DSizes<Eigen::DenseIndex, 5> src_indices;
          Eigen::DSizes<Eigen::DenseIndex, 5> src_sizes;
          Eigen::DSizes<Eigen::DenseIndex, 5> dst_indices;
          Eigen::DSizes<Eigen::DenseIndex, 5> dst_sizes;
          src.FillIndicesAndSizes<5>(out_backprop.shape(), &src_indices,
                                     &src_sizes);
          dst.FillIndicesAndSizes<5>(tensor_in.shape(), &dst_indices,
                                     &dst_sizes);

#if !defined(EIGEN_HAS_INDEX_LIST)
          Eigen::array<int, 5> bcast = {1, csize, rsize, psize, 1};
#else
          Eigen::IndexList<Eigen::type2index<1>, int, int, int,
                           Eigen::type2index<1>>
              bcast;
          bcast.set(1, csize);
          bcast.set(2, rsize);
          bcast.set(3, psize);
#endif

          // Slice from tensor_in.
          Eigen::Tensor<T, 5, Eigen::RowMajor> tensor_in_slice(dst_sizes);
          tensor_in_slice.device(context->eigen_cpu_device()) =
              tensor_in.tensor<T, 5>().slice(dst_indices, dst_sizes);

          // Slice from tensor_out.
          Eigen::Tensor<T, 5, Eigen::RowMajor> tensor_out_slice(src_sizes);
          tensor_out_slice.device(context->eigen_cpu_device()) =
              tensor_out.tensor<T, 5>().slice(src_indices, src_sizes);

          // Backprop slice.
          Eigen::Tensor<T, 5, Eigen::RowMajor> out_backprop_slice(src_sizes);
          out_backprop_slice.device(context->eigen_cpu_device()) =
              out_backprop.tensor<T, 5>().slice(src_indices, src_sizes);

          // The true backprop slice: if an element is the max, choose
          // the backprop slice; otherwise set to 0.
          Eigen::Tensor<T, 5, Eigen::RowMajor> select_slice(dst_sizes);
          Eigen::Tensor<T, 5, Eigen::RowMajor> mat0(dst_sizes);
          mat0.setZero();
          select_slice =
              ((tensor_in_slice - tensor_out_slice.broadcast(bcast)).abs() <
               tensor_in_slice.constant(1e-5))
                  .select(out_backprop_slice.broadcast(bcast), mat0);

          output->tensor<T, 5>()
              .slice(dst_indices, dst_sizes)
              .device(context->eigen_cpu_device()) += select_slice;
        }
      }
    }
  }