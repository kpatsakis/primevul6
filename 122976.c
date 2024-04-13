  static void Compute(OpKernelContext* context, const CPUDevice& device,
                      typename TTypes<T, 3>::ConstTensor& input,
                      typename TTypes<T>::Tensor& output,
                      const Eigen::Index lower_diag_index,
                      const Eigen::Index upper_diag_index,
                      const Eigen::Index max_diag_len, const T padding_value,
                      const bool left_align_superdiagonal,
                      const bool left_align_subdiagonal) {
    // 10 in cost_per_batch is from existing heuristic.
    // TODO(penporn): Tune for the best constant in cost_per_batch.
    const Eigen::Index num_diags = upper_diag_index - lower_diag_index + 1;
    const Eigen::Index output_elements_in_batch = num_diags * max_diag_len;
    const Eigen::Index cost_per_batch = 10 * output_elements_in_batch;
    const Eigen::Index num_batches = input.dimension(0);
    const Eigen::Index num_rows = input.dimension(1);
    const Eigen::Index num_cols = input.dimension(2);

    auto compute_shard = [&output, &input, &num_rows, &num_cols,
                          &upper_diag_index, &max_diag_len, &num_diags,
                          &output_elements_in_batch, &padding_value,
                          &left_align_superdiagonal, &left_align_subdiagonal](
                             Eigen::Index begin, Eigen::Index end) {
      Eigen::Index output_base_index = begin * output_elements_in_batch;
      for (Eigen::Index batch = begin; batch < end; ++batch) {
        for (Eigen::Index m = 0; m < num_diags; ++m) {
          const Eigen::Index diag_index = upper_diag_index - m;
          Eigen::Index y_offset = std::max<Eigen::Index>(0, -diag_index);
          Eigen::Index x_offset = std::max<Eigen::Index>(0, diag_index);
          int diag_len, content_offset;
          std::tie(diag_len, content_offset) = ComputeDiagLenAndContentOffset(
              diag_index, max_diag_len, num_rows, num_cols,
              left_align_superdiagonal, left_align_subdiagonal);

          // Fills the diagonal.
          for (Eigen::Index n = 0; n < diag_len; ++n) {
            output(output_base_index + content_offset + n) =
                input(batch, n + y_offset, n + x_offset);
          }

          // Padding.
          const bool left_align = (content_offset == 0);
          const Eigen::Index padding_start = (left_align) ? diag_len : 0;
          const Eigen::Index padding_end =
              (left_align) ? max_diag_len : content_offset;
          for (Eigen::Index n = padding_start; n < padding_end; ++n) {
            output(output_base_index + n) = padding_value;
          }
          output_base_index += max_diag_len;
        }
      }
    };
    auto thread_pool =
        context->device()->tensorflow_cpu_worker_threads()->workers;
    thread_pool->ParallelFor(num_batches, cost_per_batch,
                             std::move(compute_shard));
  }