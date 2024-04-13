R_API RList *r_bin_java_find_cp_const_by_val(RBinJavaObj *bin_obj, const ut8 *bytes, ut32 len, const char t) {
	switch (t) {
	case R_BIN_JAVA_CP_UTF8: return r_bin_java_find_cp_const_by_val_utf8 (bin_obj, bytes, len);
	case R_BIN_JAVA_CP_INTEGER: return r_bin_java_find_cp_const_by_val_int (bin_obj, bytes, len);
	case R_BIN_JAVA_CP_FLOAT: return r_bin_java_find_cp_const_by_val_float (bin_obj, bytes, len);
	case R_BIN_JAVA_CP_LONG: return r_bin_java_find_cp_const_by_val_long (bin_obj, bytes, len);
	case R_BIN_JAVA_CP_DOUBLE: return r_bin_java_find_cp_const_by_val_double (bin_obj, bytes, len);
	case R_BIN_JAVA_CP_UNKNOWN:
	default:
		eprintf ("Failed to perform the search for: %s\n", bytes);
		return r_list_new ();
	}
}