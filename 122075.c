R_API ut64 r_bin_java_methodref_cp_calc_size(RBinJavaCPTypeObj *obj) {
	ut64 size = 0;
	// tag
	size += 1;
	// obj->info.cp_method.class_idx = R_BIN_JAVA_USHORT (buffer, 1);
	size += 2;
	// obj->info.cp_method.name_and_type_idx = R_BIN_JAVA_USHORT (buffer, 3);
	size += 2;
	return size;
}