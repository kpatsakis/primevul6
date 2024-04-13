exif_process_user_comment(image_info_type* /*ImageInfo*/, char** pszInfoPtr,
                          char** pszEncoding, char* szValuePtr, int ByteCount) {
  int   a;

#if EXIF_USE_MBSTRING
  char  *decode;
  size_t len;
#endif

  *pszEncoding = nullptr;
  /* Copy the comment */
  if (ByteCount>=8) {
    if (!memcmp(szValuePtr, "UNICODE\0", 8)) {
      PHP_STRDUP(*pszEncoding, (const char*)szValuePtr);
      szValuePtr = szValuePtr+8;
      ByteCount -= 8;
#if EXIF_USE_MBSTRING
      /* First try to detect BOM: ZERO WIDTH NOBREAK SPACE (FEFF 16)
       * since we have no encoding support for the BOM yet we skip that.
       */
      if (!memcmp(szValuePtr, "\xFE\xFF", 2)) {
        decode = "UCS-2BE";
        szValuePtr = szValuePtr+2;
        ByteCount -= 2;
      } else if (!memcmp(szValuePtr, "\xFF\xFE", 2)) {
        decode = "UCS-2LE";
        szValuePtr = szValuePtr+2;
        ByteCount -= 2;
      } else if (ImageInfo->motorola_intel) {
        decode = ImageInfo->decode_unicode_be;
      } else {
        decode = ImageInfo->decode_unicode_le;
      }
      *pszInfoPtr = php_mb_convert_encoding(szValuePtr, ByteCount,
                                            ImageInfo->encode_unicode,
                                            decode, &len);
      return len;
#else
      return exif_process_string_raw(pszInfoPtr, szValuePtr, ByteCount);
#endif
    } else
    if (!memcmp(szValuePtr, "ASCII\0\0\0", 8)) {
      PHP_STRDUP(*pszEncoding, (const char*)szValuePtr);
      szValuePtr = szValuePtr+8;
      ByteCount -= 8;
    } else
    if (!memcmp(szValuePtr, "JIS\0\0\0\0\0", 8)) {
      /* JIS should be tanslated to MB or we leave it to the user */
      PHP_STRDUP(*pszEncoding, (const char*)szValuePtr);
      szValuePtr = szValuePtr+8;
      ByteCount -= 8;
#if EXIF_USE_MBSTRING
      if (ImageInfo->motorola_intel) {
        *pszInfoPtr = php_mb_convert_encoding(szValuePtr, ByteCount,
                                              ImageInfo->encode_jis,
                                              ImageInfo->decode_jis_be, &len);
      } else {
        *pszInfoPtr = php_mb_convert_encoding(szValuePtr, ByteCount,
                                              ImageInfo->encode_jis,
                                              ImageInfo->decode_jis_le, &len);
      }
      return len;
#else
      return exif_process_string_raw(pszInfoPtr, szValuePtr, ByteCount);
#endif
    } else
    if (!memcmp(szValuePtr, "\0\0\0\0\0\0\0\0", 8)) {
      /* 8 NULL means undefined and should be ASCII... */
      PHP_STRDUP(*pszEncoding, "UNDEFINED");
      szValuePtr = szValuePtr+8;
      ByteCount -= 8;
    }
  }

  /* Olympus has this padded with trailing spaces.  Remove these first. */
  if (ByteCount>0) {
    for (a=ByteCount-1;a && szValuePtr[a]==' ';a--) {
      (szValuePtr)[a] = '\0';
    }
  }

  /* normal text without encoding */
  exif_process_string(pszInfoPtr, szValuePtr, ByteCount);
  return strlen(*pszInfoPtr);
}