R_API void r_bin_java_print_bootstrap_methods_attr_summary(RBinJavaAttrInfo *attr) {
	RListIter *iter, *iter_tmp;
	RBinJavaBootStrapMethod *obj = NULL;
	if (attr == NULL || attr->type == R_BIN_JAVA_ATTR_TYPE_BOOTSTRAP_METHODS_ATTR) {
		eprintf ("Unable to print attribue summary for RBinJavaAttrInfo *RBinJavaBootstrapMethodsAttr");
		return;
	}
	eprintf ("Bootstrap Methods Attribute Information Information:\n");
	eprintf ("	Attribute Offset: 0x%08"PFMT64x"", attr->file_offset);
	eprintf ("	Length: 0x%08x", attr->length);
	eprintf ("	Number of Method Arguments = (0x%02x)\n", attr->info.bootstrap_methods_attr.num_bootstrap_methods);
	if (attr->info.bootstrap_methods_attr.bootstrap_methods) {
		r_list_foreach_safe (attr->info.bootstrap_methods_attr.bootstrap_methods, iter, iter_tmp, obj) {
			if (obj) {
				r_bin_java_print_bootstrap_method_summary (obj);
			}
		}
	} else {
		eprintf ("	Bootstrap Methods: NONE \n");
	}
}