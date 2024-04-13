R_API char *r_bin_java_print_integer_cp_stringify(RBinJavaCPTypeObj *obj) {
	ut32 size = 255, consumed = 0;
	char *value = malloc (size);
	if (value) {
		memset (value, 0, size);
		consumed = snprintf (value, size, "%d.0x%04"PFMT64x ".%s.0x%08x",
			obj->metas->ord, obj->file_offset + obj->loadaddr, ((RBinJavaCPTypeMetas *) obj->metas->type_info)->name,
			R_BIN_JAVA_UINT (obj->info.cp_integer.bytes.raw, 0));
		if (consumed >= size - 1) {
			free (value);
			size += size >> 1;
			value = malloc (size);
			if (value) {
				memset (value, 0, size);
				consumed = snprintf (value, size, "%d.0x%04"PFMT64x ".%s.0x%08x",
					obj->metas->ord, obj->file_offset + obj->loadaddr, ((RBinJavaCPTypeMetas *) obj->metas->type_info)->name,
					R_BIN_JAVA_UINT (obj->info.cp_integer.bytes.raw, 0));
			}
		}
	}
	return value;
}