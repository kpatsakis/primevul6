R_API RBinJavaCPTypeObj *r_bin_java_get_item_from_bin_cp_list(RBinJavaObj *bin, ut64 idx) {
	/*
	Search through the Constant Pool list for the given CP Index.
	If the idx not found by directly going to the list index,
	the list will be walked and then the IDX will be checked.
	rvalue: RBinJavaObj* (user does NOT free).
	*/
	if (bin == NULL) {
		return NULL;
	}
	if (idx > bin->cp_count || idx == 0) {
		return r_bin_java_get_java_null_cp ();
	}
	return r_bin_java_get_item_from_cp_item_list (bin->cp_list, idx);
}