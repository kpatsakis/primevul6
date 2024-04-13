R_API void r_bin_java_line_number_table_attr_free(void /*RBinJavaAttrInfo*/ *a) {
	RBinJavaAttrInfo *attr = a;
	if (attr) {
		free (attr->name);
		free (attr->metas);
		r_list_free (attr->info.line_number_table_attr.line_number_table);
		free (attr);
	}
}