R_API char *r_bin_java_resolve_cp_idx_to_string(RBinJavaObj *BIN_OBJ, int idx) {
	RBinJavaCPTypeObj *item = NULL;
	char *value = NULL;
	if (BIN_OBJ && BIN_OBJ->cp_count < 1) {
		return NULL;
	}
	item = (RBinJavaCPTypeObj *) r_bin_java_get_item_from_bin_cp_list (BIN_OBJ, idx);
	if (item) {
		value = ((RBinJavaCPTypeMetas *)
		item->metas->type_info)->
		allocs->stringify_obj (item);
	}
	return value;
}