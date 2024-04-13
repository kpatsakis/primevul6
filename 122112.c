R_API RList *r_bin_java_get_ret(RBinJavaField *fm_type) {
	RList *the_list = r_bin_java_extract_type_values (fm_type->descriptor);
	RList *ret_list = r_list_new ();
	ut8 in_ret = 0;
	RListIter *desc_iter;
	char *str;
	r_list_foreach (the_list, desc_iter, str) {
		if (str && *str != ')') {
			in_ret = 0;
		}
		if (in_ret) {
			r_list_append (ret_list, strdup (str));
		}
	}
	r_list_free (the_list);
	return ret_list;
}