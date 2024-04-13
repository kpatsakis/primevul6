R_API void r_bin_java_enclosing_methods_attr_free(void /*RBinJavaAttrInfo*/ *a) {
	RBinJavaAttrInfo *attr = a;
	if (attr) {
		free (attr->name);
		free (attr->metas);
		free (attr->info.enclosing_method_attr.class_name);
		free (attr->info.enclosing_method_attr.method_name);
		free (attr->info.enclosing_method_attr.method_descriptor);
		free (attr);
	}
}