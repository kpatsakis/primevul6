R_API void r_bin_java_classes_free(void /*RBinClass*/ *k) {
	RBinClass *klass = k;
	if (klass) {
		r_list_free (klass->methods);
		r_list_free (klass->fields);
		free (klass->name);
		free (klass->super);
		free (klass->visibility_str);
		free (klass);
	}
}