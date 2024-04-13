R_API RBinJavaCPTypeObj *r_bin_java_get_java_null_cp() {
	if (R_BIN_JAVA_NULL_TYPE_INITTED) {
		return &R_BIN_JAVA_NULL_TYPE;
	}
	memset (&R_BIN_JAVA_NULL_TYPE, 0, sizeof (R_BIN_JAVA_NULL_TYPE));
	R_BIN_JAVA_NULL_TYPE.metas = R_NEW0 (RBinJavaMetaInfo);
	if (!R_BIN_JAVA_NULL_TYPE.metas) {
		return NULL;
	}
	memset (R_BIN_JAVA_NULL_TYPE.metas, 0, sizeof (RBinJavaMetaInfo));
	R_BIN_JAVA_NULL_TYPE.metas->type_info = &R_BIN_JAVA_CP_METAS[0];
	R_BIN_JAVA_NULL_TYPE.metas->ord = 0;
	R_BIN_JAVA_NULL_TYPE.file_offset = 0;
	R_BIN_JAVA_NULL_TYPE_INITTED = true;
	return &R_BIN_JAVA_NULL_TYPE;
}