R_API void r_bin_java_bootstrap_method_free(void /*/RBinJavaBootStrapMethod*/ *b) {
	RBinJavaBootStrapMethod *bsm = b;
	RListIter *iter, *iter_tmp;
	RBinJavaBootStrapArgument *obj = NULL;
	if (bsm) {
		if (bsm->bootstrap_arguments) {
			r_list_foreach_safe (bsm->bootstrap_arguments, iter, iter_tmp, obj) {
				if (obj) {
					r_bin_java_bootstrap_method_argument_free (obj);
				}
				// r_list_delete (bsm->bootstrap_arguments, iter);
			}
			r_list_free (bsm->bootstrap_arguments);
			bsm->bootstrap_arguments = NULL;
		}
		free (bsm);
	}
}