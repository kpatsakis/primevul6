R_API RList *retrieve_all_access_string_and_value(RBinJavaAccessFlags *access_flags) {
	const char *fmt = "%s = 0x%04x";
	RList *result = r_list_new ();
	if (!result) {
		return NULL;
	}
	result->free = free;
	int i = 0;
	for (i = 0; access_flags[i].str != NULL; i++) {
		char *str = malloc (50);
		if (!str) {
			r_list_free (result);
			return NULL;
		}
		snprintf (str, 49, fmt, access_flags[i].str, access_flags[i].value);
		r_list_append (result, str);
	}
	return result;
}