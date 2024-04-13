R_API ut64 r_bin_java_class_cp_calc_size(RBinJavaCPTypeObj *obj) {
	ut64 size = 0;
	// ut8 tag = buffer[0];
	size += 1;
	// obj->info.cp_class.name_idx = R_BIN_JAVA_USHORT (buffer, 1);
	size += 2;
	return size;
}