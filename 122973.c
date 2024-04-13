  static void Compute(OpKernelContext* context, const CPUDevice& device,
                      typename TTypes<T>::ConstTensor& diag,
                      typename TTypes<T, 3>::Tensor& output,
                      const Eigen::Index lower_diag_index,
                      const Eigen::Index upper_diag_index,
                      const Eigen::Index max_diag_len, const T padding_value,
                      const bool left_align_superdiagonal,
                      const bool left_align_subdiagonal) {
    // 10 in cost_per_batch is from existing heuristic.
    // TODO(penporn): Tune for the best constant in cost_per_batch.
    const Eigen::Index num_batches = output.dimension(0);
    const Eigen::Index num_rows = output.dimension(1);
    const Eigen::Index num_cols = output.dimension(2);
    const Eigen::Index cost_per_batch = 10 * num_rows * num_cols;

    auto compute_shard = [&output, &num_rows, &num_cols, &diag,
                          &lower_diag_index, &upper_diag_index, &max_diag_len,
                          &padding_value, &left_align_superdiagonal,
                          &left_align_subdiagonal](Eigen::Index begin,
                                                   Eigen::Index end) {
      const int num_diags = upper_diag_index - lower_diag_index + 1;
      const int diag_elements_in_batch = num_diags * max_diag_len;
      Eigen::Index diag_batch_base_index = begin * diag_elements_in_batch;
      for (Eigen::Index batch = begin; batch < end; ++batch) {
        for (Eigen::Index i = 0; i < output.dimension(1); ++i) {
          for (Eigen::Index j = 0; j < output.dimension(2); ++j) {
            const int diag_index = j - i;
            const int diag_index_in_input = upper_diag_index - diag_index;
            int diag_len, content_offset;
            std::tie(diag_len, content_offset) = ComputeDiagLenAndContentOffset(
                diag_index, max_diag_len, num_rows, num_cols,
                left_align_superdiagonal, left_align_subdiagonal);
            const int index_in_the_diagonal =
                j - std::max<Eigen::Index>(diag_index, 0) + content_offset;
            if (lower_diag_index <= diag_index &&
                diag_index <= upper_diag_index) {
              output(batch, i, j) = diag(diag_batch_base_index +
                                         diag_index_in_input * max_diag_len +
                                         index_in_the_diagonal);
            } else {
              output(batch, i, j) = padding_value;
            }
          }
        }
        diag_batch_base_index += diag_elements_in_batch;
      }
    };
    auto thread_pool =
        context->device()->tensorflow_cpu_worker_threads()->workers;
    thread_pool->ParallelFor(num_batches, cost_per_batch,
                             std::move(compute_shard));
  }