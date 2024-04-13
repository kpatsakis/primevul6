R_API ut64 r_bin_java_bootstrap_methods_attr_calc_size(RBinJavaAttrInfo *attr) {
	RListIter *iter, *iter_tmp;
	RBinJavaBootStrapMethod *bsm = NULL;
	ut64 size = 0;
	if (attr) {
		size += 6;
		// attr->info.bootstrap_methods_attr.num_bootstrap_methods = R_BIN_JAVA_USHORT (buffer, offset);
		size += 2;
		r_list_foreach_safe (attr->info.bootstrap_methods_attr.bootstrap_methods, iter, iter_tmp, bsm) {
			if (bsm) {
				size += r_bin_java_bootstrap_method_calc_size (bsm);
			} else {
				// TODO eprintf Failed to read the %d boot strap method.
			}
		}
	}
	return size;
}