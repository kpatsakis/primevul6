R_API char *r_bin_java_get_this_class_name(RBinJavaObj *bin) {
	return (bin->cf2.this_class_name ? strdup (bin->cf2.this_class_name) : strdup ("unknown"));
}