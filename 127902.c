static int exif_file_sections_free(image_info_type *ImageInfo)
{
	int i;

	if (ImageInfo->file.count) {
		for (i=0; i<ImageInfo->file.count; i++) {
			EFREE_IF(ImageInfo->file.list[i].data);
		}
	}
	EFREE_IF(ImageInfo->file.list);
	ImageInfo->file.count = 0;
	return TRUE;
}