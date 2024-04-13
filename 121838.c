R_API ut8 *r_bin_java_cp_append_classref_and_name(RBinJavaObj *bin, ut32 *out_sz, const char *classname, const ut32 classname_len) {
	ut16 use_name_idx = bin->cp_idx + 1;
	ut8 *bytes = NULL, *name_bytes = NULL;
	name_bytes = r_bin_java_cp_get_utf8 (R_BIN_JAVA_CP_UTF8, out_sz, (const ut8 *) classname, classname_len);
	if (*out_sz > 0 && name_bytes) {
		ut8 *idx_addr = (ut8 *) &use_name_idx;
		bytes = malloc (*out_sz + 3);
		memcpy (bytes, name_bytes, *out_sz);
		bytes[*out_sz + 0] = R_BIN_JAVA_CP_CLASS;
		bytes[*out_sz + 1] = idx_addr[1];
		bytes[*out_sz + 2] = idx_addr[0];
		*out_sz += 3;
	}
	free (name_bytes);
	return bytes;
}