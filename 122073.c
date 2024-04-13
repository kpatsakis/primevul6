R_API void r_bin_java_local_variable_type_table_attr_free(void /*RBinJavaAttrInfo*/ *a) {
	RBinJavaAttrInfo *attr = a;
	if (attr) {
		free (attr->name);
		free (attr->metas);
		r_list_free (attr->info.local_variable_type_table_attr.local_variable_table);
		free (attr);
	}
}