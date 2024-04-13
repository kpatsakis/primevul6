R_API void r_bin_java_set_imports(RBinJavaObj *bin) {
	RListIter *iter = NULL;
	RBinJavaCPTypeObj *obj = NULL;
	r_list_free (bin->imports_list);
	bin->imports_list = r_list_newf (free);
	r_list_foreach (bin->cp_list, iter, obj) {
		const char *type = NULL;
		switch (obj->tag) {
		case R_BIN_JAVA_CP_METHODREF: type = "METHOD"; break;
		case R_BIN_JAVA_CP_INTERFACEMETHOD_REF: type = "FIELD"; break;
		case R_BIN_JAVA_CP_FIELDREF: type = "INTERFACE_METHOD"; break;
		default: type = NULL; break;
		}
		if (type) {
			r_bin_add_import (bin, obj, type);
		}
	}
}