R_API RBinJavaCPTypeObj *r_bin_java_clone_cp_idx(RBinJavaObj *bin, ut32 idx) {
	RBinJavaCPTypeObj *obj = NULL;
	if (bin) {
		obj = r_bin_java_get_item_from_bin_cp_list (bin, idx);
	}
	return r_bin_java_clone_cp_item (obj);
}