R_API ut64 r_bin_java_resolve_cp_idx_address(RBinJavaObj *BIN_OBJ, int idx) {
	RBinJavaCPTypeObj *item = NULL;
	ut64 addr = -1;
	if (BIN_OBJ && BIN_OBJ->cp_count < 1) {
		return -1;
	}
	item = (RBinJavaCPTypeObj *) r_bin_java_get_item_from_bin_cp_list (BIN_OBJ, idx);
	if (item) {
		addr = item->file_offset + item->loadaddr;
	}
	return addr;
}