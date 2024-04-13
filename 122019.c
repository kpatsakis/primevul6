R_API char *r_bin_java_resolve_cp_idx_type(RBinJavaObj *BIN_OBJ, int idx) {
	RBinJavaCPTypeObj *item = NULL;
	char *str = NULL;
	if (BIN_OBJ && BIN_OBJ->cp_count < 1) {
		// r_bin_java_new_bin(BIN_OBJ);
		return NULL;
	}
	item = (RBinJavaCPTypeObj *) r_bin_java_get_item_from_bin_cp_list (BIN_OBJ, idx);
	if (item) {
		str = strdup (((RBinJavaCPTypeMetas *) item->metas->type_info)->name);
	} else {
		str = strdup ("INVALID");
	}
	return str;
}