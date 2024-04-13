R_API ut64 r_bin_java_utf8_cp_calc_size(RBinJavaCPTypeObj *obj) {
	ut64 size = 0;
	size += 1;
	if (obj && R_BIN_JAVA_CP_UTF8 == obj->tag) {
		size += 2;
		size += obj->info.cp_utf8.length;
	}
	return size;
}