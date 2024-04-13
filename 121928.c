R_API RBinSymbol *r_bin_java_create_new_symbol_from_cp_idx(ut32 cp_idx, ut64 baddr) {
	RBinSymbol *sym = NULL;
	RBinJavaCPTypeObj *obj = r_bin_java_get_item_from_bin_cp_list (
		R_BIN_JAVA_GLOBAL_BIN, cp_idx);
	if (obj) {
		switch (obj->tag) {
		case R_BIN_JAVA_CP_METHODREF:
		case R_BIN_JAVA_CP_FIELDREF:
		case R_BIN_JAVA_CP_INTERFACEMETHOD_REF:
			sym = r_bin_java_create_new_symbol_from_ref (obj, baddr);
			break;
		case R_BIN_JAVA_CP_INVOKEDYNAMIC:
			sym = r_bin_java_create_new_symbol_from_invoke_dynamic (obj, baddr);
			break;
		default:
			break;
		}
	}
	return sym;
}