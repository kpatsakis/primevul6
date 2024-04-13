R_API RList *r_bin_java_get_args(RBinJavaField *fm_type) {
	RList *the_list = r_bin_java_extract_type_values (fm_type->descriptor);
	RList *arg_list = r_list_new ();
	ut8 in_args = 0;
	RListIter *desc_iter;
	char *str;
	r_list_foreach (the_list, desc_iter, str) {
		if (str && *str == '(') {
			in_args = 1;
			continue;
		}
		if (str && *str == ')') {
			break;
		}
		if (in_args) {
			r_list_append (arg_list, strdup (str));
		}
	}
	r_list_free (the_list);
	return arg_list;
}