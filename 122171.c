R_API RBinJavaBootStrapArgument *r_bin_java_bootstrap_method_argument_new(ut8 *buffer, ut64 sz, ut64 buf_offset) {
	RBinJavaBootStrapArgument *bsm_arg = NULL;
	ut64 offset = 0;
	bsm_arg = (RBinJavaBootStrapArgument *) malloc (sizeof (RBinJavaBootStrapArgument));
	if (bsm_arg == NULL) {
		// TODO eprintf failed to allocate bytes for bootstrap_method.
		return bsm_arg;
	}
	memset (bsm_arg, 0, sizeof (RBinJavaBootStrapArgument));
	bsm_arg->file_offset = buf_offset;
	bsm_arg->argument_info_idx = R_BIN_JAVA_USHORT (buffer, offset);
	offset += 2;
	bsm_arg->argument_info_cp_obj = r_bin_java_clone_cp_idx (R_BIN_JAVA_GLOBAL_BIN, bsm_arg->argument_info_idx);
	bsm_arg->size = offset;
	return bsm_arg;
}