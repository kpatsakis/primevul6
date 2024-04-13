R_API int U(r_bin_java_integer_cp_set)(RBinJavaObj * bin, ut16 idx, ut32 val) {
	RBinJavaCPTypeObj *cp_obj = r_bin_java_get_item_from_bin_cp_list (bin, idx);
	if (!cp_obj) {
		return false;
	}
	ut8 bytes[4] = {
		0
	};
	if (cp_obj->tag != R_BIN_JAVA_CP_INTEGER && cp_obj->tag != R_BIN_JAVA_CP_FLOAT) {
		eprintf ("Not supporting the overwrite of CP Objects with one of a different size.\n");
		return false;
	}
	r_bin_java_check_reset_cp_obj (cp_obj, R_BIN_JAVA_CP_INTEGER);
	cp_obj->tag = R_BIN_JAVA_CP_INTEGER;
	memcpy (bytes, (const char *) &val, 4);
	val = R_BIN_JAVA_UINT (bytes, 0);
	memcpy (&cp_obj->info.cp_integer.bytes.raw, (const char *) &val, 4);
	return true;
}