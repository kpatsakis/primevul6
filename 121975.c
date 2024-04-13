R_API ut32 r_bin_java_get_utf8_len_from_bin_cp_list(RBinJavaObj *bin, ut64 idx) {
	/*
	Search through the Constant Pool list for the given CP Index.
	If the idx not found by directly going to the list index,
	the list will be walked and then the IDX will be checked.
	rvalue: new char* for caller to free.
	*/
	if (bin == NULL) {
		return 0;
	}
	return r_bin_java_get_utf8_len_from_cp_item_list (bin->cp_list, idx);
}