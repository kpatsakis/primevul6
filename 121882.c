R_API RBinJavaElementValueMetas *r_bin_java_get_ev_meta_from_tag(ut8 tag) {
	ut16 i = 0;
	RBinJavaElementValueMetas *res = &R_BIN_JAVA_ELEMENT_VALUE_METAS[13];
	for (i = 0; i < R_BIN_JAVA_ELEMENT_VALUE_METAS_SZ; i++) {
		if (tag == R_BIN_JAVA_ELEMENT_VALUE_METAS[i].tag) {
			res = &R_BIN_JAVA_ELEMENT_VALUE_METAS[i];
			break;
		}
	}
	return res;
}