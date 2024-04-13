R_API int U(r_bin_java_get_cp_idx_with_name)(RBinJavaObj * bin_obj, const char *name, ut32 len) {
	RListIter *iter;
	RBinJavaCPTypeObj *obj;
	r_list_foreach (bin_obj->cp_list, iter, obj) {
		if (obj->tag == R_BIN_JAVA_CP_UTF8) {
			if (!strncmp (name, (const char *) obj->info.cp_utf8.bytes, len)) {
				return obj->metas->ord;
			}
		}
	}
	return 0;
}