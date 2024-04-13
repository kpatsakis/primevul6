R_API RBinJavaCPTypeObj *r_bin_java_integer_cp_new(RBinJavaObj *bin, ut8 *buffer, ut64 sz) {
	ut8 tag = buffer[0];
	RBinJavaCPTypeObj *obj = NULL;
	int quick_check = 0;
	quick_check = r_bin_java_quick_check (R_BIN_JAVA_CP_INTEGER, tag, sz, "Integer");
	if (quick_check > 0) {
		return obj;
	}
	obj = (RBinJavaCPTypeObj *) R_NEW0 (RBinJavaCPTypeObj);
	if (obj) {
		obj->tag = tag;
		obj->metas = R_NEW0 (RBinJavaMetaInfo);
		obj->metas->type_info = (void *) &R_BIN_JAVA_CP_METAS[tag];
		obj->name = r_str_dup (NULL, (const char *) R_BIN_JAVA_CP_METAS[tag].name);
		memset (&obj->info.cp_integer.bytes, 0, sizeof (obj->info.cp_integer.bytes));
		memcpy (&obj->info.cp_integer.bytes.raw, buffer + 1, 4);

	}
	return obj;
}