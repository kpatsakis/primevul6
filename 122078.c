R_API RBinJavaAttrMetas *r_bin_java_get_attr_type_by_name(const char *name) {
	// TODO: use sdb/hashtable here
	int i;
	for (i = 0; i < RBIN_JAVA_ATTRS_METAS_SZ; i++) {
		if (!strcmp ((const char *) name, RBIN_JAVA_ATTRS_METAS[i].name)) {
			return &RBIN_JAVA_ATTRS_METAS[i];
		}
	}
	return &RBIN_JAVA_ATTRS_METAS[R_BIN_JAVA_ATTR_TYPE_UNKNOWN_ATTR];
}