R_API DsoJsonObj *r_bin_java_get_fm_type_definition_json(RBinJavaObj *bin, RBinJavaField *fm_type, int is_method) {
	ut64 addr = UT64_MAX;
	char *prototype = NULL, *fq_name = NULL;
	bool is_native = ((fm_type->flags & R_BIN_JAVA_METHOD_ACC_NATIVE) != 0);
	bool is_static = ((fm_type->flags & R_BIN_JAVA_METHOD_ACC_STATIC) != 0);
	bool is_synthetic = ((fm_type->flags & R_BIN_JAVA_METHOD_ACC_SYNTHETIC) != 0);
	bool is_private = ((fm_type->flags & R_BIN_JAVA_METHOD_ACC_PRIVATE) != 0);
	bool is_public = ((fm_type->flags & R_BIN_JAVA_METHOD_ACC_PUBLIC) != 0);
	bool is_protected = ((fm_type->flags & R_BIN_JAVA_METHOD_ACC_PROTECTED) != 0);
	bool is_super = ((fm_type->flags & R_BIN_JAVA_CLASS_ACC_SUPER) != 0);

	DsoJsonObj *fm_type_dict = dso_json_dict_new ();
	dso_json_dict_insert_str_key_num (fm_type_dict, "access_flags", fm_type->flags);
	dso_json_dict_insert_str_key_num (fm_type_dict, "is_method", is_method);
	dso_json_dict_insert_str_key_num (fm_type_dict, "is_native", is_native);
	dso_json_dict_insert_str_key_num (fm_type_dict, "is_synthetic", is_synthetic);
	dso_json_dict_insert_str_key_num (fm_type_dict, "is_private", is_private);
	dso_json_dict_insert_str_key_num (fm_type_dict, "is_public", is_public);
	dso_json_dict_insert_str_key_num (fm_type_dict, "is_static", is_static);
	dso_json_dict_insert_str_key_num (fm_type_dict, "is_protected", is_protected);
	dso_json_dict_insert_str_key_num (fm_type_dict, "is_super", is_super);

	addr = r_bin_java_get_method_code_offset (fm_type);
	if (addr == 0) {
		addr = fm_type->file_offset;
	}
	addr += bin->loadaddr;

	dso_json_dict_insert_str_key_num (fm_type_dict, "addr", addr);
	dso_json_dict_insert_str_key_num (fm_type_dict, "offset", fm_type->file_offset + bin->loadaddr);
	dso_json_dict_insert_str_key_str (fm_type_dict, "class_name", fm_type->class_name);
	dso_json_dict_insert_str_key_str (fm_type_dict, "signature", fm_type->descriptor);
	dso_json_dict_insert_str_key_str (fm_type_dict, "name", fm_type->name);

	if (is_method) {
		fq_name = r_bin_java_create_method_fq_str (fm_type->class_name, fm_type->name, fm_type->descriptor);
	} else {
		fq_name = r_bin_java_create_field_fq_str (fm_type->class_name, fm_type->name, fm_type->descriptor);
	}
	dso_json_dict_insert_str_key_str (fm_type_dict, "fq_name", fq_name);

	prototype = r_bin_java_unmangle (fm_type->flags_str, fm_type->name, fm_type->descriptor);
	dso_json_dict_insert_str_key_str (fm_type_dict, "prototype", prototype);
	free (prototype);
	free (fq_name);
	return fm_type_dict;
}