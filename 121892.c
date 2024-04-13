R_API RList *r_bin_java_find_cp_const_by_val_long(RBinJavaObj *bin_obj, const ut8 *bytes, ut32 len) {
	RList *res = r_list_newf (free);
	ut32 *v = NULL;
	RListIter *iter;
	RBinJavaCPTypeObj *cp_obj;
	eprintf ("Looking for 0x%08x\n", R_BIN_JAVA_UINT (bytes, 0));
	r_list_foreach (bin_obj->cp_list, iter, cp_obj) {
		if (cp_obj->tag == R_BIN_JAVA_CP_LONG) {
			if (len == 8 && r_bin_java_raw_to_long (cp_obj->info.cp_long.bytes.raw, 0) == r_bin_java_raw_to_long (bytes, 0)) {
				// TODO: we can safely store a ut32 inside the list without having to allocate it
				v = malloc (sizeof (ut32));
				if (!v) {
					r_list_free (res);
					return NULL;
				}
				*v = cp_obj->idx;
				r_list_append (res, v);
			}
		}
	}
	return res;
}