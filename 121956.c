R_API ut64 r_bin_java_bootstrap_method_calc_size(RBinJavaBootStrapMethod *bsm) {
	RListIter *iter, *iter_tmp;
	RBinJavaBootStrapArgument *bsm_arg = NULL;
	ut64 size = 0;
	if (bsm) {
		size += 6;
		// bsm->bootstrap_method_ref = R_BIN_JAVA_USHORT (buffer, offset);
		size += 2;
		// bsm->num_bootstrap_arguments = R_BIN_JAVA_USHORT (buffer, offset);
		size += 2;
		r_list_foreach_safe (bsm->bootstrap_arguments, iter, iter_tmp, bsm_arg) {
			if (bsm_arg) {
				size += r_bin_java_bootstrap_arg_calc_size (bsm_arg);
			} else {
				// TODO eprintf Failed to read the %d boot strap method.
			}
		}
	}
	return size;
}