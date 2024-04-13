R_API const RList *r_bin_java_get_methods_list(RBinJavaObj *bin) {
	if (bin) {
		return bin->methods_list;
	}
	if (R_BIN_JAVA_GLOBAL_BIN) {
		return R_BIN_JAVA_GLOBAL_BIN->methods_list;
	}
	return NULL;
}