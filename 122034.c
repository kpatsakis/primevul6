R_API RBinJavaCPTypeObj *r_bin_java_methodref_cp_new(RBinJavaObj *bin, ut8 *buffer, ut64 sz) {
	ut8 tag = buffer[0];
	RBinJavaCPTypeObj *obj = NULL;
	int quick_check = 0;
	quick_check = r_bin_java_quick_check (R_BIN_JAVA_CP_METHODREF, tag, sz, "MethodRef");
	if (quick_check > 0) {
		return obj;
	}
	obj = (RBinJavaCPTypeObj *) malloc (sizeof (RBinJavaCPTypeObj));
	if (obj) {
		memset (obj, 0, sizeof (RBinJavaCPTypeObj));
		obj->tag = tag;
		obj->metas = R_NEW0 (RBinJavaMetaInfo);
		obj->metas->type_info = (void *) &R_BIN_JAVA_CP_METAS[tag];
		obj->info.cp_method.class_idx = R_BIN_JAVA_USHORT (buffer, 1);
		obj->info.cp_method.name_and_type_idx = R_BIN_JAVA_USHORT (buffer, 3);
	}
	return obj;
}