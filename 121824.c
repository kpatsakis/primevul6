R_API int r_bin_java_check_reset_cp_obj(RBinJavaCPTypeObj *cp_obj, ut8 tag) {
	bool res = false;
	if (tag < R_BIN_JAVA_CP_METAS_SZ) {
		if (tag != cp_obj->tag) {
			if (cp_obj->tag == R_BIN_JAVA_CP_UTF8) {
				free (cp_obj->info.cp_utf8.bytes);
				cp_obj->info.cp_utf8.bytes = NULL;
				cp_obj->info.cp_utf8.length = 0;
				free (cp_obj->name);
				cp_obj->name = NULL;
				res = true;
			}
			cp_obj->tag = tag;
			cp_obj->metas->type_info = (void *) &R_BIN_JAVA_CP_METAS[tag];
			cp_obj->name = strdup (R_BIN_JAVA_CP_METAS[tag].name);
			res = true;
		} else {
			eprintf ("Invalid tag\n");
		}
	} else {
		eprintf ("Invalid tag '%d'.\n", tag);
	}
	return res;
}