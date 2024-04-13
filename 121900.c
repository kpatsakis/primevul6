R_API RList *U(r_bin_java_get_fields)(RBinJavaObj * bin) {
	RListIter *iter = NULL, *iter_tmp = NULL;
	RList *fields = r_list_new ();
	RBinJavaField *fm_type;
	RBinField *field;
	r_list_foreach_safe (bin->fields_list, iter, iter_tmp, fm_type) {
		field = r_bin_java_create_new_rbinfield_from_field (fm_type, bin->loadaddr);
		if (field) {
			r_list_append (fields, field);
		}
	}
	return fields;
}