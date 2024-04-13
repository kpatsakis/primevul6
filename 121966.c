R_API RList *r_bin_java_get_import_definitions(RBinJavaObj *bin) {
	RList *the_list = r_bin_java_get_lib_names (bin);
	RListIter *iter = NULL;
	char *new_str;
	if (!bin || !the_list) {
		return the_list;
	}
	r_list_foreach (the_list, iter, new_str) {
		while (*new_str) {
			if (*new_str == '/') {
				*new_str = '.';
			}
			new_str++;
		}
	}
	return the_list;
}