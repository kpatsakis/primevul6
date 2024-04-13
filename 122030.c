R_API void r_bin_java_stack_map_table_attr_free(void /*RBinJavaAttrInfo*/ *a) {
	RBinJavaAttrInfo *attr = a;
	if (attr) {
		free (attr->name);
		free (attr->metas);
		r_list_free (attr->info.stack_map_table_attr.stack_map_frame_entries);
		free (attr);
	}
}