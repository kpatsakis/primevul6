R_API int U(r_bin_java_utf8_cp_set)(RBinJavaObj * bin, ut16 idx, const ut8 * buffer, ut32 len) {
	RBinJavaCPTypeObj *cp_obj = r_bin_java_get_item_from_bin_cp_list (bin, idx);
	if (!cp_obj) {
		return false;
	}
	eprintf ("Writing %d byte(s) (%s)\n", len, buffer);
	// r_bin_java_check_reset_cp_obj(cp_obj, R_BIN_JAVA_CP_INTEGER);
	if (cp_obj->tag != R_BIN_JAVA_CP_UTF8) {
		eprintf ("Not supporting the overwrite of CP Objects with one of a different size.\n");
		return false;
	}
	if (cp_obj->info.cp_utf8.length != len) {
		eprintf ("Not supporting the resize, rewriting utf8 string up to %d byte(s).\n", cp_obj->info.cp_utf8.length);
		if (cp_obj->info.cp_utf8.length > len) {
			eprintf ("Remaining %d byte(s) will be filled with \\x00.\n", cp_obj->info.cp_utf8.length - len);
		}
	}
	memcpy (cp_obj->info.cp_utf8.bytes, buffer, cp_obj->info.cp_utf8.length);
	if (cp_obj->info.cp_utf8.length > len) {
		memset (cp_obj->info.cp_utf8.bytes + len, 0, cp_obj->info.cp_utf8.length - len);
	}
	return true;
}