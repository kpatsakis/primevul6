  static void launch(OpKernelContext* context, const Pool3dParameters& params,
                     const Tensor& tensor_in, const Tensor& tensor_out,
                     const Tensor& tensor_top_diff,
                     Tensor* tensor_bottom_diff) {
    OP_REQUIRES(
        context, params.data_format == FORMAT_NHWC,
        errors::InvalidArgument("Default MaxPooling3dGradGradOp only supports",
                                "NDHWC on CPU device type"));

    typedef Eigen::Map<const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>>
        ConstEigenMatrixMap;
    typedef Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>>
        EigenMatrixMap;

    ConstEigenMatrixMap in_mat(tensor_in.flat<T>().data(), params.depth,
                               params.tensor_in_planes * params.tensor_in_cols *
                                   params.tensor_in_rows *
                                   params.tensor_in_batch);
    ConstEigenMatrixMap out_mat(tensor_out.flat<T>().data(), params.depth,
                                params.out_plane * params.out_width *
                                    params.out_height * params.tensor_in_batch);
    ConstEigenMatrixMap top_diff_mat(
        tensor_top_diff.flat<T>().data(), params.depth,
        params.tensor_in_planes * params.tensor_in_cols *
            params.tensor_in_rows * params.tensor_in_batch);
    EigenMatrixMap bottom_diff_mat(
        tensor_bottom_diff->flat<T>().data(), params.depth,
        params.out_plane * params.out_width * params.out_height *
            params.tensor_in_batch);

    const DeviceBase::CpuWorkerThreads& worker_threads =
        *(context->device()->tensorflow_cpu_worker_threads());

    auto shard = [&params, &in_mat, &out_mat, &top_diff_mat, &bottom_diff_mat](
                     int64 start, int64 limit) {
      const int32 depth = params.depth;
      const int32 in_planes = params.tensor_in_planes;
      const int32 in_rows = params.tensor_in_rows;
      const int32 in_cols = params.tensor_in_cols;
      const int32 pad_planes = params.pad_planes;
      const int32 pad_rows = params.pad_rows;
      const int32 pad_cols = params.pad_cols;
      const int32 window_planes = params.window_planes;
      const int32 window_rows = params.window_rows;
      const int32 window_cols = params.window_cols;
      const int32 plane_stride = params.plane_stride;
      const int32 row_stride = params.row_stride;
      const int32 col_stride = params.col_stride;
      const int32 out_plane = params.out_plane;
      const int32 out_height = params.out_height;
      const int32 out_width = params.out_width;

      {
        // Initializes the output grad backprop tensor with 0.
        const int32 output_image_size =
            out_plane * out_height * out_width * params.depth;
        EigenMatrixMap bottom_diff_shard(
            bottom_diff_mat.data() + start * output_image_size, 1,
            (limit - start) * output_image_size);
        bottom_diff_shard.setZero();
      }

      for (int b = start; b < limit; ++b) {
        for (int pp = 0; pp < out_plane; ++pp) {
          for (int ph = 0; ph < out_height; ++ph) {
            for (int pw = 0; pw < out_width; ++pw) {
              // (p_start, p_end) * (h_start, h_end) * (w_start, w_end) is the
              // range that the input vector projects to.
              int p_start = pp * plane_stride - pad_planes;
              const int p_end = std::min(p_start + window_planes, in_planes);
              int h_start = ph * row_stride - pad_rows;
              const int h_end = std::min(h_start + window_rows, in_rows);
              int w_start = pw * col_stride - pad_cols;
              const int w_end = std::min(w_start + window_cols, in_cols);
              p_start = std::max(p_start, 0);
              h_start = std::max(h_start, 0);
              w_start = std::max(w_start, 0);
              const int out_index =
                  ((b * out_plane + pp) * out_height + ph) * out_width + pw;
              // Find value corresponding to the input maximum in top_diff.
              for (int d = 0; d < depth; ++d) {
                const T& output_ref = out_mat.coeffRef(d, out_index);
                bool should_stop = false;
                for (int p = p_start; p < p_end && !should_stop; ++p) {
                  for (int h = h_start; h < h_end && !should_stop; ++h) {
                    for (int w = w_start; w < w_end && !should_stop; ++w) {
                      const int in_index =
                          ((b * in_planes + p) * in_rows + h) * in_cols + w;
                      const T& input_ref = in_mat.coeffRef(d, in_index);
                      if (output_ref == input_ref) {
                        T& bottom_diff_ref =
                            bottom_diff_mat.coeffRef(d, out_index);
                        bottom_diff_ref = top_diff_mat.coeffRef(d, in_index);
                        should_stop = true;
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    };
    const int64 shard_cost =
        params.out_plane * params.out_height * params.out_width * params.depth *
        params.window_planes * params.window_rows * params.window_cols;
    Shard(worker_threads.num_threads, worker_threads.workers,
          params.tensor_in_batch, shard_cost, shard);
  }