void Im2col(const T* input_data, const int depth, const int height,
            const int width, const int filter_h, const int filter_w,
            const int pad_t, const int pad_l, const int pad_b, const int pad_r,
            const int stride_h, const int stride_w, T* col_data) {
  int height_col = (height + pad_t + pad_b - filter_h) / stride_h + 1;
  int width_col = (width + pad_l + pad_r - filter_w) / stride_w + 1;

  int h_pad = -pad_t;
  for (int h = 0; h < height_col; ++h) {
    int w_pad = -pad_l;
    for (int w = 0; w < width_col; ++w) {
      for (int ih = h_pad; ih < h_pad + filter_h; ++ih) {
        for (int iw = w_pad; iw < w_pad + filter_w; ++iw) {
          if (ih >= 0 && ih < height && iw >= 0 && iw < width) {
            memcpy(col_data, input_data + (ih * width + iw) * depth,
                   sizeof(T) * depth);
          } else {
            // This should be simply padded with zero.
            memset(col_data, 0, sizeof(T) * depth);
          }
          col_data += depth;
        }
      }
      w_pad += stride_w;
    }
    h_pad += stride_h;
  }
}