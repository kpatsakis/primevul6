exif_process_unicode(image_info_type* /*ImageInfo*/, xp_field_type* xp_field,
                     int tag, char* szValuePtr, int ByteCount) {
  xp_field->tag = tag;
  xp_field->value = nullptr;

  /* Copy the comment */
#if EXIF_USE_MBSTRING
  xp_field->value =
    php_mb_convert_encoding(szValuePtr, ByteCount, ImageInfo->encode_unicode,
                            ImageInfo->decode_unicode_le, &xp_field->size);
  return xp_field->size;
#else
  xp_field->size =
    exif_process_string_raw(&xp_field->value, szValuePtr, ByteCount);
  return xp_field->size;
#endif
}