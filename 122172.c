R_API RList *r_bin_java_get_field_num_name(RBinJavaObj *bin_obj) {
	ut32 i = 0;
	RBinJavaField *fm_type;
	RListIter *iter = NULL;
	RList *res = r_list_newf (free);
	r_list_foreach (bin_obj->fields_list, iter, fm_type) {
		ut32 len = strlen (fm_type->name) + 30;
		char *str = malloc (len);
		if (!str) {
			r_list_free (res);
			return NULL;
		}
		snprintf (str, len, "%d %s", i, fm_type->name);
		++i;
		r_list_append (res, str);
	}
	return res;
}