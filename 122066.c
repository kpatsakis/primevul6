R_API ut64 r_bin_java_name_and_type_cp_calc_size(RBinJavaCPTypeObj *obj) {
	ut64 size = 0;
	if (obj) {
		size += 1;
		// obj->info.cp_name_and_type.name_idx = R_BIN_JAVA_USHORT (buffer, 1);
		size += 2;
		// obj->info.cp_name_and_type.descriptor_idx = R_BIN_JAVA_USHORT (buffer, 3);
		size += 2;
	}
	return size;
}