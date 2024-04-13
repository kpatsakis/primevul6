R_API char *r_bin_java_create_method_fq_str(const char *klass, const char *name, const char *signature) {
	if (!klass) {
		klass = "null_class";
	}
	if (!name) {
		name = "null_name";
	}
	if (!signature) {
		signature = "null_signature";
	}
	return r_str_newf ("%s.%s.%s", klass, name, signature);
}