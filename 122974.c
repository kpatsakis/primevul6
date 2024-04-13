std::pair<int, int> ComputeDiagLenAndContentOffset(
    int diag_index, int max_diag_len, int num_rows, int num_cols,
    bool left_align_superdiagonal, bool left_align_subdiagonal) {
  const bool left_align = (diag_index >= 0 && left_align_superdiagonal) ||
                          (diag_index <= 0 && left_align_subdiagonal);
  const int diag_len = std::min(num_rows + std::min(0, diag_index),
                                num_cols - std::max(0, diag_index));
  const int content_offset = (left_align) ? 0 : (max_diag_len - diag_len);
  return {diag_len, content_offset};
}