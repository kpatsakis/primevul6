R_API RList *r_bin_java_get_imports(RBinJavaObj *bin) {
	RList *ret = r_list_newf (free);
	RBinImport *import = NULL;
	RListIter *iter;
	r_list_foreach (bin->imports_list, iter, import) {
		RBinImport *n_import = R_NEW0 (RBinImport);
		if (!n_import) {
			r_list_free (ret);
			return NULL;
		}
		memcpy (n_import, import, sizeof (RBinImport));
		r_list_append (ret, n_import);
	}
	return ret;
}