R_API char *r_bin_java_print_name_and_type_cp_stringify(RBinJavaCPTypeObj *obj) {
	ut32 size = 255, consumed = 0;
	char *value = malloc (size);
	if (value) {
		memset (value, 0, size);
		consumed = snprintf (value, size, "%d.0x%04"PFMT64x ".%s.%d.%d",
			obj->metas->ord, obj->file_offset + obj->loadaddr, ((RBinJavaCPTypeMetas *) obj->metas->type_info)->name,
			obj->info.cp_name_and_type.name_idx,
			obj->info.cp_name_and_type.descriptor_idx);
		if (consumed >= size - 1) {
			free (value);
			size += size >> 1;
			value = malloc (size);
			if (value) {
				memset (value, 0, size);
				consumed = snprintf (value, size, "%d.0x%04"PFMT64x ".%s.%d.%d",
					obj->metas->ord, obj->file_offset + obj->loadaddr, ((RBinJavaCPTypeMetas *) obj->metas->type_info)->name,
					obj->info.cp_name_and_type.name_idx,
					obj->info.cp_name_and_type.descriptor_idx);
			}
		}
	}
	return value;
}