R_API int U(r_bin_java_long_cp_set)(RBinJavaObj * bin, ut16 idx, ut64 val) {
	RBinJavaCPTypeObj *cp_obj = r_bin_java_get_item_from_bin_cp_list (bin, idx);
	if (!cp_obj) {
		return false;
	}
	ut8 bytes[8] = {
		0
	};
	if (cp_obj->tag != R_BIN_JAVA_CP_LONG && cp_obj->tag != R_BIN_JAVA_CP_DOUBLE) {
		eprintf ("Not supporting the overwrite of CP Objects with one of a different size.\n");
		return false;
	}
	r_bin_java_check_reset_cp_obj (cp_obj, R_BIN_JAVA_CP_LONG);
	cp_obj->tag = R_BIN_JAVA_CP_LONG;
	memcpy (bytes, (const char *) &val, 8);
	val = r_bin_java_raw_to_long (bytes, 0);
	memcpy (&cp_obj->info.cp_long.bytes.raw, (const char *) &val, 8);
	return true;
}