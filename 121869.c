R_API char *r_bin_java_get_field_name(RBinJavaObj *bin_obj, ut32 idx) {
	char *name = NULL;
	if (idx < r_list_length (bin_obj->fields_list)) {
		RBinJavaField *fm_type = r_list_get_n (bin_obj->fields_list, idx);
		name = strdup (fm_type->name);
	}
	return name;
}