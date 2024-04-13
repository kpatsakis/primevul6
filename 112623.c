static char *php_unicode_convert_case(int case_mode, const char *srcstr,
                                      size_t srclen, unsigned int *ret_len,
                                      const char *src_encoding) {
  char *unicode, *newstr;
  unsigned int unicode_len;
  unsigned char *unicode_ptr;
  size_t i;
  enum mbfl_no_encoding _src_encoding = mbfl_name2no_encoding(src_encoding);

  unicode = php_mb_convert_encoding(srcstr, srclen, "UCS-4BE", src_encoding,
                                    &unicode_len);
  if (unicode == nullptr)
    return nullptr;

  unicode_ptr = (unsigned char *)unicode;

  switch(case_mode) {
  case PHP_UNICODE_CASE_UPPER:
    for (i = 0; i < unicode_len; i+=4) {
      UINT32_TO_BE_ARY(&unicode_ptr[i],
                       php_unicode_toupper(BE_ARY_TO_UINT32(&unicode_ptr[i]),
                                           _src_encoding));
    }
    break;

  case PHP_UNICODE_CASE_LOWER:
    for (i = 0; i < unicode_len; i+=4) {
      UINT32_TO_BE_ARY(&unicode_ptr[i],
                       php_unicode_tolower(BE_ARY_TO_UINT32(&unicode_ptr[i]),
                                           _src_encoding));
    }
    break;

  case PHP_UNICODE_CASE_TITLE:
    {
      int mode = 0;

      for (i = 0; i < unicode_len; i+=4) {
        int res = php_unicode_is_prop
          (BE_ARY_TO_UINT32(&unicode_ptr[i]),
           UC_MN|UC_ME|UC_CF|UC_LM|UC_SK|UC_LU|UC_LL|UC_LT|UC_PO|UC_OS, 0);
        if (mode) {
          if (res) {
            UINT32_TO_BE_ARY
              (&unicode_ptr[i],
               php_unicode_tolower(BE_ARY_TO_UINT32(&unicode_ptr[i]),
                                   _src_encoding));
          } else {
            mode = 0;
          }
        } else {
          if (res) {
            mode = 1;
            UINT32_TO_BE_ARY
              (&unicode_ptr[i],
               php_unicode_totitle(BE_ARY_TO_UINT32(&unicode_ptr[i]),
                                   _src_encoding));
          }
        }
      }
    }
    break;
  }

  newstr = php_mb_convert_encoding(unicode, unicode_len, src_encoding,
                                   "UCS-4BE", ret_len);
  free(unicode);
  return newstr;
}