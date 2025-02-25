static void exif_iif_add_int(image_info_type *image_info, int section_index, char *name, int value)
{
	image_info_data  *info_data;
	image_info_data  *list;

	list = safe_erealloc(image_info->info_list[section_index].list, (image_info->info_list[section_index].count+1), sizeof(image_info_data), 0);
	image_info->info_list[section_index].list = list;

	info_data  = &image_info->info_list[section_index].list[image_info->info_list[section_index].count];
	info_data->tag    = TAG_NONE;
	info_data->format = TAG_FMT_SLONG;
	info_data->length = 1;
	info_data->name   = estrdup(name);
	info_data->value.i = value;
	image_info->sections_found |= 1<<section_index;
	image_info->info_list[section_index].count++;
}