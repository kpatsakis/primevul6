R_API char *r_bin_java_unmangle_without_flags(const char *name, const char *descriptor) {
	return r_bin_java_unmangle (NULL, name, descriptor);
}