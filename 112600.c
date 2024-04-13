static size_t php_mb_mbchar_bytes_ex(const char *s, const mbfl_encoding *enc) {
  if (enc != nullptr) {
    if (enc->flag & MBFL_ENCTYPE_MBCS) {
      if (enc->mblen_table != nullptr) {
        if (s != nullptr) return enc->mblen_table[*(unsigned char *)s];
      }
    } else if (enc->flag & (MBFL_ENCTYPE_WCS2BE | MBFL_ENCTYPE_WCS2LE)) {
      return 2;
    } else if (enc->flag & (MBFL_ENCTYPE_WCS4BE | MBFL_ENCTYPE_WCS4LE)) {
      return 4;
    }
  }
  return 1;
}