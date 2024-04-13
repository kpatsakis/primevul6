R_API int r_bin_java_valid_class(const ut8 *buf, ut64 buf_sz) {
	RBinJavaObj *bin = R_NEW0 (RBinJavaObj), *cur_bin = R_BIN_JAVA_GLOBAL_BIN;
	if (!bin) {
		return false;
	}
	int res = r_bin_java_load_bin (bin, buf, buf_sz);
	if (bin->calc_size == buf_sz) {
		res = true;
	}
	r_bin_java_free (bin);
	R_BIN_JAVA_GLOBAL_BIN = cur_bin;
	return res;
}