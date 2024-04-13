R_API char *r_bin_java_print_double_cp_stringify(RBinJavaCPTypeObj *obj) {
	ut32 size = 255, consumed = 0;
	char *value = malloc (size);
	if (value) {
		memset (value, 0, size);
		consumed = snprintf (value, size, "%d.0x%04"PFMT64x ".%s.%f",
			obj->metas->ord,
			obj->file_offset,
			((RBinJavaCPTypeMetas *) obj->metas->type_info)->name,
			r_bin_java_raw_to_double (obj->info.cp_double.bytes.raw, 0));
		if (consumed >= size - 1) {
			free (value);
			size += size >> 1;
			value = malloc (size);
			if (value) {
				memset (value, 0, size);
				consumed = snprintf (value, size, "%d.0x%04"PFMT64x ".%s.%f",
					obj->metas->ord,
					obj->file_offset,
					((RBinJavaCPTypeMetas *) obj->metas->type_info)->name,
					r_bin_java_raw_to_double (obj->info.cp_double.bytes.raw, 0));
			}
		}
	}
	return value;
}