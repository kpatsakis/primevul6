R_API RList *r_bin_java_find_cp_const_by_val_double(RBinJavaObj *bin_obj, const ut8 *bytes, ut32 len) {
	RList *res = r_list_newf (free);
	ut32 *v = NULL;
	RListIter *iter;
	RBinJavaCPTypeObj *cp_obj;
	eprintf ("Looking for %f\n", r_bin_java_raw_to_double (bytes, 0));
	r_list_foreach (bin_obj->cp_list, iter, cp_obj) {
		if (cp_obj->tag == R_BIN_JAVA_CP_DOUBLE) {
			if (len == 8 && r_bin_java_raw_to_double (cp_obj->info.cp_long.bytes.raw, 0) == r_bin_java_raw_to_double (bytes, 0)) {
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