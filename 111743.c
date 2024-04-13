static RList *libs(RBinFile *bf) {
	int i;
	char *ptr = NULL;
	struct lib_t *libs;
	RList *ret = NULL;
	RBinObject *obj = bf ? bf->o : NULL;

	if (!obj || !obj->bin_obj || !(ret = r_list_newf (free))) {
		return NULL;
	}
	if ((libs = MACH0_(get_libs) (obj->bin_obj))) {
		for (i = 0; !libs[i].last; i++) {
			ptr = strdup (libs[i].name);
			r_list_append (ret, ptr);
		}
		free (libs);
	}
	return ret;
}