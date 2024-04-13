R_API RBinJavaCPTypeObj *r_bin_java_interfacemethodref_cp_new(RBinJavaObj *bin, ut8 *buffer, ut64 sz) {
	ut8 tag = buffer[0];
	int quick_check = r_bin_java_quick_check (R_BIN_JAVA_CP_INTERFACEMETHOD_REF, tag, sz, "InterfaceMethodRef");
	if (quick_check > 0) {
		return NULL;
	}
	RBinJavaCPTypeObj *obj = R_NEW0 (RBinJavaCPTypeObj);
	if (obj) {
		obj->tag = tag;
		obj->metas = R_NEW0 (RBinJavaMetaInfo);
		obj->metas->type_info = (void *) &R_BIN_JAVA_CP_METAS[tag];
		obj->name = r_str_dup (NULL, (const char *) R_BIN_JAVA_CP_METAS[tag].name);
		obj->info.cp_interface.class_idx = R_BIN_JAVA_USHORT (buffer, 1);
		obj->info.cp_interface.name_and_type_idx = R_BIN_JAVA_USHORT (buffer, 3);

	}
	return obj;
}