fixup_resource_directory (char *res_section, char *p, guint32 rva)
{
	MonoPEResourceDir *dir = (MonoPEResourceDir*)p;
	int i;

	p += sizeof (MonoPEResourceDir);
	for (i = 0; i < GUINT16_FROM_LE (dir->res_named_entries) + GUINT16_FROM_LE (dir->res_id_entries); ++i) {
		MonoPEResourceDirEntry *dir_entry = (MonoPEResourceDirEntry*)p;
		char *child = res_section + MONO_PE_RES_DIR_ENTRY_DIR_OFFSET (*dir_entry);
		if (MONO_PE_RES_DIR_ENTRY_IS_DIR (*dir_entry)) {
			fixup_resource_directory (res_section, child, rva);
		} else {
			MonoPEResourceDataEntry *data_entry = (MonoPEResourceDataEntry*)child;
			data_entry->rde_data_offset = GUINT32_TO_LE (GUINT32_FROM_LE (data_entry->rde_data_offset) + rva);
		}

		p += sizeof (MonoPEResourceDirEntry);
	}
}