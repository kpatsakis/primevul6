R_API ut8 *r_bin_java_cp_get_idx_bytes(RBinJavaObj *bin, ut16 idx, ut32 *out_sz) {
	RBinJavaCPTypeObj *cp_obj = r_bin_java_get_item_from_bin_cp_list (bin, idx);
	if (!cp_obj || !out_sz) {
		return NULL;
	}
	if (out_sz) {
		*out_sz = 0;
	}
	switch (cp_obj->tag) {
	case R_BIN_JAVA_CP_INTEGER:
	case R_BIN_JAVA_CP_FLOAT:
		return r_bin_java_cp_get_4bytes (cp_obj->tag, out_sz, cp_obj->info.cp_integer.bytes.raw, 5);
	case R_BIN_JAVA_CP_LONG:
	case R_BIN_JAVA_CP_DOUBLE:
		return r_bin_java_cp_get_4bytes (cp_obj->tag, out_sz, cp_obj->info.cp_long.bytes.raw, 9);
	case R_BIN_JAVA_CP_UTF8:
		// eprintf ("Getting idx: %d = %p (3+0x%"PFMT64x")\n", idx, cp_obj, cp_obj->info.cp_utf8.length);
		if (cp_obj->info.cp_utf8.length > 0) {
			return r_bin_java_cp_get_utf8 (cp_obj->tag, out_sz,
				cp_obj->info.cp_utf8.bytes, cp_obj->info.cp_utf8.length);
		}
	}
	return NULL;
}