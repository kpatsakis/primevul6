calc_section_size (MonoDynamicImage *assembly)
{
	int nsections = 0;

	/* alignment constraints */
	mono_image_add_stream_zero (&assembly->code, 4 - (assembly->code.index % 4));
	g_assert ((assembly->code.index % 4) == 0);
	assembly->meta_size += 3;
	assembly->meta_size &= ~3;
	mono_image_add_stream_zero (&assembly->resources, 4 - (assembly->resources.index % 4));
	g_assert ((assembly->resources.index % 4) == 0);

	assembly->sections [MONO_SECTION_TEXT].size = assembly->meta_size + assembly->code.index + assembly->resources.index + assembly->strong_name_size;
	assembly->sections [MONO_SECTION_TEXT].attrs = SECT_FLAGS_HAS_CODE | SECT_FLAGS_MEM_EXECUTE | SECT_FLAGS_MEM_READ;
	nsections++;

	if (assembly->win32_res) {
		guint32 res_size = (assembly->win32_res_size + 3) & ~3;

		assembly->sections [MONO_SECTION_RSRC].size = res_size;
		assembly->sections [MONO_SECTION_RSRC].attrs = SECT_FLAGS_HAS_INITIALIZED_DATA | SECT_FLAGS_MEM_READ;
		nsections++;
	}

	assembly->sections [MONO_SECTION_RELOC].size = 12;
	assembly->sections [MONO_SECTION_RELOC].attrs = SECT_FLAGS_MEM_READ | SECT_FLAGS_MEM_DISCARDABLE | SECT_FLAGS_HAS_INITIALIZED_DATA;
	nsections++;

	return nsections;
}