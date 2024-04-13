R_API ut64 r_bin_java_invokedynamic_cp_calc_size(RBinJavaCPTypeObj *obj) {
	ut64 size = 0;
	size += 1;
	// obj->info.cp_invoke_dynamic.bootstrap_method_attr_index = R_BIN_JAVA_USHORT (buffer, 1);
	size += 2;
	// obj->info.cp_invoke_dynamic.name_and_type_index = R_BIN_JAVA_USHORT (buffer, 3);
	size += 2;
	return size;
}