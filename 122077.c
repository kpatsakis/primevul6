R_API RBinJavaCPTypeObj *r_bin_java_invokedynamic_cp_new(RBinJavaObj *bin, ut8 *buffer, ut64 sz) {
	ut8 tag = buffer[0];
	RBinJavaCPTypeObj *obj;
	int quick_check = r_bin_java_quick_check (R_BIN_JAVA_CP_INVOKEDYNAMIC, tag, sz, "RBinJavaCPTypeMethodHandle");
	if (quick_check > 0) {
		return NULL;
	}
	if ((obj = R_NEW0 (RBinJavaCPTypeObj))) {
		obj->metas = R_NEW0 (RBinJavaMetaInfo);
		obj->metas->type_info = (void *) &R_BIN_JAVA_CP_METAS[tag];
		obj->name = r_str_dup (NULL, (const char *) R_BIN_JAVA_CP_METAS[tag].name);;
		obj->tag = tag;
		obj->info.cp_invoke_dynamic.bootstrap_method_attr_index = R_BIN_JAVA_USHORT (buffer, 1);
		obj->info.cp_invoke_dynamic.name_and_type_index = R_BIN_JAVA_USHORT (buffer, 3);
	}
	return obj;
}