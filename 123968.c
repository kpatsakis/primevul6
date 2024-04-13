  static void Compute(OpKernelContext* context, const CPUDevice& device,
                      typename TTypes<T, 3>::ConstTensor& input,
                      typename TTypes<T>::ConstTensor& diag,
                      typename TTypes<T, 3>::Tensor& output,
                      const Eigen::Index lower_diag_index,
                      const Eigen::Index upper_diag_index,
                      const Eigen::Index max_diag_len,
                      const bool left_align_superdiagonal,
                      const bool left_align_subdiagonal) {
    if (input.data() != output.data()) {
      output.device(device) = input;
    }
    const Eigen::Index num_diags = upper_diag_index - lower_diag_index + 1;
    auto compute_shard = [&output, &diag, &upper_diag_index, &max_diag_len,
                          &num_diags, &left_align_superdiagonal,
                          &left_align_subdiagonal](Eigen::Index begin,
                                                   Eigen::Index end) {
      const Eigen::Index num_rows = output.dimension(1);
      const Eigen::Index num_cols = output.dimension(2);
      Eigen::Index diag_base_index = begin * num_diags * max_diag_len;
      for (Eigen::Index batch = begin; batch < end; ++batch) {
        for (Eigen::Index m = 0; m < num_diags; ++m) {
          const Eigen::Index diag_index = upper_diag_index - m;
          int diag_len, content_offset;
          std::tie(diag_len, content_offset) = ComputeDiagLenAndContentOffset(
              diag_index, max_diag_len, num_rows, num_cols,
              left_align_superdiagonal, left_align_subdiagonal);

          // Make two separate cases to save some index calculations.
          if (diag_index >= 0) {
            for (Eigen::Index n = 0; n < diag_len; ++n) {
              output(batch, n, n + diag_index) =
                  diag(diag_base_index + n + content_offset);
            }
          } else {
            for (Eigen::Index n = 0; n < diag_len; ++n) {
              output(batch, n - diag_index, n) =
                  diag(diag_base_index + n + content_offset);
            }
          }
          diag_base_index += max_diag_len;
        }
      }
    };
    auto thread_pool =
        context->device()->tensorflow_cpu_worker_threads()->workers;
    // TODO(penporn): Tune for the best constant in cost_per_batch.
    const Eigen::Index cost_per_batch = 10 * num_diags * max_diag_len;
    thread_pool->ParallelFor(output.dimension(0), cost_per_batch,
                             std::move(compute_shard));
  }