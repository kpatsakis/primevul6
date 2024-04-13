R_API RBinJavaCPTypeObj *r_bin_java_utf8_cp_new(RBinJavaObj *bin, ut8 *buffer, ut64 sz) {
	ut8 tag = buffer[0];
	RBinJavaCPTypeObj *obj;
	int quick_check = r_bin_java_quick_check (R_BIN_JAVA_CP_UTF8, tag, sz, "Utf8");
	if (quick_check > 0) {
		return NULL;
	}
	if ((obj = R_NEW0 (RBinJavaCPTypeObj))) {
		memset (obj, 0, sizeof (RBinJavaCPTypeObj));
		obj->tag = tag;
		obj->metas = R_NEW0 (RBinJavaMetaInfo);
		obj->metas->type_info = (void *) &R_BIN_JAVA_CP_METAS[tag];
		obj->name = r_str_dup (NULL, (const char *) R_BIN_JAVA_CP_METAS[tag].name);
		obj->info.cp_utf8.length = R_BIN_JAVA_USHORT (buffer, 1);
		obj->info.cp_utf8.bytes = (ut8 *) malloc (obj->info.cp_utf8.length + 1);
		if (obj->info.cp_utf8.bytes) {
			memset (obj->info.cp_utf8.bytes, 0, obj->info.cp_utf8.length + 1);
			if (obj->info.cp_utf8.length < (sz - 3)) {
				memcpy (obj->info.cp_utf8.bytes, buffer + 3, (sz - 3));
				obj->info.cp_utf8.length = sz - 3;
			} else {
				memcpy (obj->info.cp_utf8.bytes, buffer + 3, obj->info.cp_utf8.length);
			}
			obj->value = obj->info.cp_utf8.bytes;
		} else {
			r_bin_java_obj_free (obj);
			obj = NULL;
		}
	}
	return obj;
}