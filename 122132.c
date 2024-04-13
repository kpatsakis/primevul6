R_API RBinSymbol *r_bin_java_create_new_symbol_from_invoke_dynamic(RBinJavaCPTypeObj *obj, ut64 baddr) {
	if (!obj || (obj->tag != R_BIN_JAVA_CP_INVOKEDYNAMIC)) {
		return NULL;
	}
	return r_bin_java_create_new_symbol_from_cp_idx (obj->info.cp_invoke_dynamic.name_and_type_index, baddr);
}