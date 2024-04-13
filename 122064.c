R_API ut8 *r_bin_java_cp_get_bytes(ut8 tag, ut32 *out_sz, const ut8 *buf, const ut64 len) {
	if (!out_sz) {
		return NULL;
	}
	if (out_sz) {
		*out_sz = 0;
	}
	switch (tag) {
	case R_BIN_JAVA_CP_INTEGER:
	case R_BIN_JAVA_CP_FLOAT:
		return r_bin_java_cp_get_4bytes (tag, out_sz, buf, len);
	case R_BIN_JAVA_CP_LONG:
	case R_BIN_JAVA_CP_DOUBLE:
		return r_bin_java_cp_get_8bytes (tag, out_sz, buf, len);
	case R_BIN_JAVA_CP_UTF8:
		return r_bin_java_cp_get_utf8 (tag, out_sz, buf, len);
	}
	return NULL;
}