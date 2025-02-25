static int exif_process_unicode(image_info_type *ImageInfo, xp_field_type *xp_field, int tag, char *szValuePtr, int ByteCount TSRMLS_DC)
{
	xp_field->tag = tag;
	xp_field->value = NULL;
	/* XXX this will fail again if encoding_converter returns on error something different than SIZE_MAX   */
	if (zend_multibyte_encoding_converter(
			(unsigned char**)&xp_field->value,
			&xp_field->size,
			(unsigned char*)szValuePtr,
			ByteCount,
			zend_multibyte_fetch_encoding(ImageInfo->encode_unicode TSRMLS_CC),
			zend_multibyte_fetch_encoding(ImageInfo->motorola_intel ? ImageInfo->decode_unicode_be : ImageInfo->decode_unicode_le TSRMLS_CC)
			TSRMLS_CC) == (size_t)-1) {
		xp_field->size = exif_process_string_raw(&xp_field->value, szValuePtr, ByteCount);
	}
	return xp_field->size;
}