R_API void r_bin_java_print_bootstrap_method_summary(RBinJavaBootStrapMethod *bsm) {
	RBinJavaBootStrapArgument *bsm_arg = NULL;
	RListIter *iter = NULL, *iter_tmp = NULL;
	if (bsm == NULL) {
		eprintf ("Attempting to print an invalid RBinJavaBootStrapArgument *.\n");
		return;
	}
	eprintf ("Bootstrap Method Information:\n");
	eprintf ("	Offset: 0x%08"PFMT64x"", bsm->file_offset);
	eprintf ("	Method Reference Index = (0x%02x)\n", bsm->bootstrap_method_ref);
	eprintf ("	Number of Method Arguments = (0x%02x)\n", bsm->num_bootstrap_arguments);
	if (bsm->bootstrap_arguments) {
		r_list_foreach_safe (bsm->bootstrap_arguments, iter, iter_tmp, bsm_arg) {
			if (bsm_arg) {
				r_bin_java_print_bootstrap_method_argument_summary (bsm_arg);
			}
		}
	} else {
		eprintf ("	Bootstrap Method Argument: NONE \n");
	}
}