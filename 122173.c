R_API ut64 r_bin_java_methodhandle_cp_calc_size(RBinJavaCPTypeObj *obj) {
	ut64 size = 0;
	size += 1;
	// obj->info.cp_method_handle.reference_index =  R_BIN_JAVA_USHORT (buffer, 2);
	size += 2;
	return size;
}