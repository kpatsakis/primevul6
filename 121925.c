R_API RList *r_bin_java_get_field_definitions(RBinJavaObj *bin) {
	RBinJavaField *fm_type = NULL;
	RList *the_list = r_list_new ();
	if (!the_list) {
		return NULL;
	}
	RListIter *iter = NULL;
	if (!bin) {
		return the_list;
	}
	r_list_foreach (bin->fields_list, iter, fm_type) {
		char *field_def = r_bin_java_get_field_definition (fm_type);
		// eprintf ("Field def: %s, %s, %s, %s\n", fm_type->name, fm_type->descriptor, fm_type->flags_str, field_def);
		r_list_append (the_list, field_def);
	}
	return the_list;
}