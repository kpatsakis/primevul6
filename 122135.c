R_API ut64 r_bin_java_methodtype_cp_calc_size(RBinJavaCPTypeObj *obj) {
	ut64 size = 0;
	size += 1;
	// obj->info.cp_method_type.descriptor_index = R_BIN_JAVA_USHORT (buffer, 1);
	size += 2;
	return size;
}