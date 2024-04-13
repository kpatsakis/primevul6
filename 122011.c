R_API int r_bin_java_resolve_cp_idx_print_summary(RBinJavaObj *BIN_OBJ, int idx) {
	RBinJavaCPTypeObj *item = NULL;
	if (BIN_OBJ && BIN_OBJ->cp_count < 1) {
		return false;
	}
	item = (RBinJavaCPTypeObj *) r_bin_java_get_item_from_bin_cp_list (BIN_OBJ, idx);
	if (item) {
		((RBinJavaCPTypeMetas *)
		item->metas->type_info)->
		allocs->print_summary (item);
	} else {
		eprintf ("Error: Invalid CP Object.\n");
	}
	return item ? true : false;
}