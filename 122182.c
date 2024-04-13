R_API RList *r_bin_java_get_strings(RBinJavaObj *bin) {
	RList *strings = r_list_newf (free);
	RBinString *str = NULL;
	RListIter *iter = NULL, *iter_tmp = NULL;
	RBinJavaCPTypeObj *cp_obj = NULL;
	r_list_foreach_safe (bin->cp_list, iter, iter_tmp, cp_obj) {
		if (cp_obj && cp_obj->tag == R_BIN_JAVA_CP_UTF8) {
			str = (RBinString *) R_NEW0 (RBinString);
			if (str) {
				str->paddr = cp_obj->file_offset + bin->loadaddr;
				str->ordinal = cp_obj->metas->ord;
				str->size = cp_obj->info.cp_utf8.length + 3;
				str->length = cp_obj->info.cp_utf8.length;
				if (str->size > 0) {
					str->string = r_str_ndup ((const char *)
						cp_obj->info.cp_utf8.bytes,
						R_BIN_JAVA_MAXSTR);
				}
				r_list_append (strings, (void *) str);
			}
		}
	}
	return strings;
}