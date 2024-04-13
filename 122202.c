R_API char *r_bin_java_resolve_without_space(RBinJavaObj *obj, int idx) {
	return r_bin_java_resolve (obj, idx, 0);
}