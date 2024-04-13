R_API RList *r_bin_java_extract_type_values(const char *arg_str) {
	RList *list_args = r_list_new ();
	if (!list_args) {
		return NULL;
	}
	char *str = NULL;
	const char *str_cur_pos = NULL;
	ut32 len = 0;
	if (!arg_str) {
		return list_args;
	}
	str_cur_pos = arg_str;
	list_args->free = free;
	while (str_cur_pos && *str_cur_pos) {
		// handle the end of an object
		len = extract_type_value (str_cur_pos, &str);
		if (len < 1) {
			r_list_free (list_args);
			return NULL;
		}
		str_cur_pos += len;
		r_list_append (list_args, str);
		str = NULL;
	}
	return list_args;
}