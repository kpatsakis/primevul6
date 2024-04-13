R_API ut8 *r_bin_java_cp_get_classref(RBinJavaObj *bin, ut32 *out_sz, const char *classname, const ut32 classname_len, const ut16 name_idx) {
	ut16 use_name_idx = -1;
	ut8 *bytes = NULL;
	if (name_idx == (ut16) - 1 && classname && *classname && classname_len > 0) {
		// find class_name_idx by class name
		RList *results = r_bin_java_find_cp_const_by_val_utf8 (bin, (const ut8 *) classname, classname_len);
		if (r_list_length (results) == 1) {
			use_name_idx = (ut16) * ((ut32 *) r_list_get_n (results, 0));
		}
		r_list_free (results);
	} else if (name_idx != (ut16) - 1 && name_idx != 0) {
		use_name_idx = name_idx;
	}
	if (use_name_idx == (ut16) - 1 && classname && *classname && classname_len > 0) {
		bytes = r_bin_java_cp_append_classref_and_name (bin, out_sz, classname, classname_len);
	} else if (use_name_idx != (ut16) - 1) {
		ut8 *idx_addr = (ut8 *) &use_name_idx;
		bytes = malloc (3);
		if (!bytes) {
			return NULL;
		}
		bytes[0] = R_BIN_JAVA_CP_CLASS;
		bytes[1] = idx_addr[1];
		bytes[2] = idx_addr[0];
		*out_sz += 3;
	}
	return bytes;
}