void r_bin_dydlcache_get_libname(struct r_bin_dyldcache_lib_t *lib, char **libname) {
	char *cur = lib->path;
	char *res = lib->path;
	int path_length = strlen (lib->path);
	while (cur < cur + path_length - 1) {
		cur = strchr (cur, '/');
		if (!cur) {
			break;
		}
		cur++;
		res = cur;
	}
	*libname = res;
}