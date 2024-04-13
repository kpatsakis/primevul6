static int exif_discard_imageinfo(image_info_type *ImageInfo)
{
	int i;

	EFREE_IF(ImageInfo->FileName);
	EFREE_IF(ImageInfo->UserComment);
	EFREE_IF(ImageInfo->UserCommentEncoding);
	EFREE_IF(ImageInfo->Copyright);
	EFREE_IF(ImageInfo->CopyrightPhotographer);
	EFREE_IF(ImageInfo->CopyrightEditor);
	EFREE_IF(ImageInfo->Thumbnail.data);
	EFREE_IF(ImageInfo->encode_unicode);
	EFREE_IF(ImageInfo->decode_unicode_be);
	EFREE_IF(ImageInfo->decode_unicode_le);
	EFREE_IF(ImageInfo->encode_jis);
	EFREE_IF(ImageInfo->decode_jis_be);
	EFREE_IF(ImageInfo->decode_jis_le);
	EFREE_IF(ImageInfo->make);
	EFREE_IF(ImageInfo->model);
	for (i=0; i<ImageInfo->xp_fields.count; i++) {
		EFREE_IF(ImageInfo->xp_fields.list[i].value);
	}
	EFREE_IF(ImageInfo->xp_fields.list);
	for (i=0; i<SECTION_COUNT; i++) {
		exif_iif_free(ImageInfo, i);
	}
	exif_file_sections_free(ImageInfo);
	memset(ImageInfo, 0, sizeof(*ImageInfo));
	return TRUE;
}