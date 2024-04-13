static int exif_file_sections_add(image_info_type *ImageInfo, int type, size_t size, uchar *data)
{
	file_section    *tmp;
	int             count = ImageInfo->file.count;

	tmp = safe_erealloc(ImageInfo->file.list, (count+1), sizeof(file_section), 0);
	ImageInfo->file.list = tmp;
	ImageInfo->file.list[count].type = 0xFFFF;
	ImageInfo->file.list[count].data = NULL;
	ImageInfo->file.list[count].size = 0;
	ImageInfo->file.count = count+1;
	if (!size) {
		data = NULL;
	} else if (data == NULL) {
		data = safe_emalloc(size, 1, 0);
	}
	ImageInfo->file.list[count].type = type;
	ImageInfo->file.list[count].data = data;
	ImageInfo->file.list[count].size = size;
	return count;
}