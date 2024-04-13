R_API void r_bin_java_inner_classes_attr_entry_free(void /*RBinJavaClassesAttribute*/ *a) {
	RBinJavaClassesAttribute *attr = a;
	if (attr) {
		free (attr->name);
		free (attr->flags_str);
		free (attr);
	}
}