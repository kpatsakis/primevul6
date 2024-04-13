R_API void r_bin_java_print_bootstrap_method_argument_summary(RBinJavaBootStrapArgument *bsm_arg) {
	if (bsm_arg == NULL) {
		eprintf ("Attempting to print an invalid RBinJavaBootStrapArgument *.\n");
		return;
	}
	eprintf ("Bootstrap Method Argument Information:\n");
	eprintf ("	Offset: 0x%08"PFMT64x"", bsm_arg->file_offset);
	eprintf ("	Name_And_Type Index = (0x%02x)\n", bsm_arg->argument_info_idx);
	if (bsm_arg->argument_info_cp_obj) {
		eprintf ("	Bootstrap Method Argument Type and Name Info:\n");
		((RBinJavaCPTypeMetas *) bsm_arg->argument_info_cp_obj)->allocs->print_summary (bsm_arg->argument_info_cp_obj);
	} else {
		eprintf ("	Bootstrap Method Argument Type and Name Info: INVALID\n");
	}
}