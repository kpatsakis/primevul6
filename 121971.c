R_API ut64 r_bin_java_get_method_code_size(RBinJavaField *fm_type) {
	RListIter *attr_iter = NULL, *attr_iter_tmp = NULL;
	RBinJavaAttrInfo *attr = NULL;
	ut64 sz = 0;
	r_list_foreach_safe (fm_type->attributes, attr_iter, attr_iter_tmp, attr) {
		if (attr->type == R_BIN_JAVA_ATTR_TYPE_CODE_ATTR) {
			sz = attr->info.code_attr.code_length;
			break;
		}
	}
	return sz;
}