R_API RList *r_bin_java_get_method_definitions(RBinJavaObj *bin) {
	RBinJavaField *fm_type = NULL;
	RList *the_list = r_list_new ();
	if (!the_list) {
		return NULL;
	}
	RListIter *iter = NULL;
	if (!bin) {
		return the_list;
	}
	r_list_foreach (bin->methods_list, iter, fm_type) {
		char *method_proto = r_bin_java_get_method_definition (fm_type);
		// eprintf ("Method prototype: %s\n", method_proto);
		r_list_append (the_list, method_proto);
	}
	return the_list;
}