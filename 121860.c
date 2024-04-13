R_API void r_bin_java_bootstrap_method_argument_free(void /*RBinJavaBootStrapArgument*/ *b) {
	RBinJavaBootStrapArgument *bsm_arg = b;
	if (bsm_arg) {
		if (bsm_arg->argument_info_cp_obj) {
			((RBinJavaCPTypeMetas *) bsm_arg->argument_info_cp_obj)->allocs->delete_obj (bsm_arg->argument_info_cp_obj);
			bsm_arg->argument_info_cp_obj = NULL;
		}
		free (bsm_arg);
	}
}