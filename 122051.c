R_API ut64 r_bin_java_bootstrap_arg_calc_size(RBinJavaBootStrapArgument *bsm_arg) {
	ut64 size = 0;
	if (bsm_arg) {
		// bsm_arg->argument_info_idx = R_BIN_JAVA_USHORT (buffer, offset);
		size += 2;
	}
	return size;
}