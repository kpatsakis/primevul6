R_API void r_bin_java_local_variable_type_table_attr_entry_free(void /*RBinJavaLocalVariableTypeAttribute*/ *a) {
	RBinJavaLocalVariableTypeAttribute *attr = a;
	if (attr) {
		free (attr->name);
		free (attr->signature);
		free (attr);
	}
}