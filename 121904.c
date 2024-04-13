R_API void r_bin_java_local_variable_table_attr_entry_free(void /*RBinJavaLocalVariableAttribute*/ *a) {
	RBinJavaLocalVariableAttribute *lvattr = a;
	if (lvattr) {
		free (lvattr->descriptor);
		free (lvattr->name);
		free (lvattr);
	}
}