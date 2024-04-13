R_API RBinJavaCPTypeObj *r_bin_java_unknown_cp_new(RBinJavaObj *bin, ut8 *buffer, ut64 sz) {
	ut8 tag = buffer[0];
	RBinJavaCPTypeObj *obj = NULL;
	obj = (RBinJavaCPTypeObj *) malloc (sizeof (RBinJavaCPTypeObj));
	if (obj) {
		memset (obj, 0, sizeof (RBinJavaCPTypeObj));
		obj->tag = tag;
		obj->metas = R_NEW0 (RBinJavaMetaInfo);
		obj->metas->type_info = (void *) &R_BIN_JAVA_CP_METAS[R_BIN_JAVA_CP_UNKNOWN];
	}
	return obj;
}