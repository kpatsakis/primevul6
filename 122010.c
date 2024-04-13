R_API ut64 r_bin_java_calc_class_size(ut8 *bytes, ut64 size) {
	RBinJavaObj *bin = R_NEW0 (RBinJavaObj);
	if (!bin) {
		return false;
	}
	RBinJavaObj *cur_bin = R_BIN_JAVA_GLOBAL_BIN;
	ut64 bin_size = UT64_MAX;
	if (bin) {
		if (r_bin_java_load_bin (bin, bytes, size)) {
			bin_size = bin->calc_size;
		}
		r_bin_java_free (bin);
		R_BIN_JAVA_GLOBAL_BIN = cur_bin;
	}
	return bin_size;
}