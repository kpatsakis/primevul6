R_API void r_bin_add_import(RBinJavaObj *bin, RBinJavaCPTypeObj *obj, const char *type) {
	RBinImport *imp = R_NEW0 (RBinImport);
	char *class_name = r_bin_java_get_name_from_bin_cp_list (bin, obj->info.cp_method.class_idx);
	char *name = r_bin_java_get_name_from_bin_cp_list (bin, obj->info.cp_method.name_and_type_idx);
	char *descriptor = r_bin_java_get_desc_from_bin_cp_list (bin, obj->info.cp_method.name_and_type_idx);
	class_name = class_name ? class_name : strdup ("INVALID CLASS NAME INDEX");
	name = name ? name : strdup ("InvalidNameIndex");
	descriptor = descriptor ? descriptor : strdup ("INVALID DESCRIPTOR INDEX");
	imp->classname = class_name;
	imp->name = name;
	imp->bind = r_str_const ("NONE");
	imp->type = r_str_const (type);
	imp->descriptor = descriptor;
	imp->ordinal = obj->idx;
	r_list_append (bin->imports_list, imp);
}