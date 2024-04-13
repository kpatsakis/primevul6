R_API RBinJavaCPTypeObj *r_bin_java_get_item_from_cp(RBinJavaObj *bin, int i) {
	if (i < 1 || i > bin->cf.cp_count) {
		return &R_BIN_JAVA_NULL_TYPE;
	}
	RBinJavaCPTypeObj *obj = (RBinJavaCPTypeObj *) r_list_get_n (bin->cp_list, i);
	return obj ? obj : &R_BIN_JAVA_NULL_TYPE;
}