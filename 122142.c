R_API RBinJavaCPTypeMetas *U(r_bin_java_get_cp_meta_from_tag)(ut8 tag) {
	ut16 i = 0;
	// set default to unknown.
	RBinJavaCPTypeMetas *res = &R_BIN_JAVA_CP_METAS[2];
	for (i = 0; i < R_BIN_JAVA_CP_METAS_SZ; i++) {
		if (tag == R_BIN_JAVA_CP_METAS[i].tag) {
			res = &R_BIN_JAVA_CP_METAS[i];
			break;
		}
	}
	return res;
}