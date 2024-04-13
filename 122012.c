R_API RBinJavaBootStrapMethod *r_bin_java_bootstrap_method_new(ut8 *buffer, ut64 sz, ut64 buf_offset) {
	RBinJavaBootStrapMethod *bsm = NULL;
	RBinJavaBootStrapArgument *bsm_arg = NULL;
	ut32 i = 0;
	ut64 offset = 0;
	bsm = (RBinJavaBootStrapMethod *) malloc (sizeof (RBinJavaBootStrapMethod));
	if (bsm == NULL) {
		// TODO eprintf failed to allocate bytes for bootstrap_method.
		return bsm;
	}
	memset (bsm, 0, sizeof (RBinJavaBootStrapMethod));
	bsm->file_offset = buf_offset;
	bsm->bootstrap_method_ref = R_BIN_JAVA_USHORT (buffer, offset);
	offset += 2;
	bsm->num_bootstrap_arguments = R_BIN_JAVA_USHORT (buffer, offset);
	offset += 2;
	bsm->bootstrap_arguments = r_list_new ();
	for (i = 0; i < bsm->num_bootstrap_arguments; i++) {
		// bsm_arg = r_bin_java_bootstrap_method_argument_new (bin, bin->b->cur);
		bsm_arg = r_bin_java_bootstrap_method_argument_new (buffer + offset, sz - offset, buf_offset + offset);
		if (bsm_arg) {
			offset += bsm_arg->size;
			r_list_append (bsm->bootstrap_arguments, (void *) bsm_arg);
		} else {
			// TODO eprintf Failed to read the %d boot strap method.
		}
	}
	bsm->size = offset;
	return bsm;
}