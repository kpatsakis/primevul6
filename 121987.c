R_API DsoJsonObj *r_bin_java_get_bin_obj_json(RBinJavaObj *bin) {
	DsoJsonObj *imports_list = r_bin_java_get_import_json_definitions (bin);
	DsoJsonObj *fields_list = r_bin_java_get_field_json_definitions (bin);
	DsoJsonObj *methods_list = r_bin_java_get_method_json_definitions (bin);
	// interfaces_list = r_bin_java_get_interface_json_definitions (bin);
	DsoJsonObj *class_dict = r_bin_java_get_class_info_json (bin);
	char *res = dso_json_obj_to_str (methods_list);
	// eprintf ("Resulting methods json: \n%s\n", res);
	free (res);
	dso_json_dict_insert_str_key_obj (class_dict, "methods", methods_list);
	// dso_json_list_free (methods_list);
	dso_json_obj_del (methods_list);

	res = dso_json_obj_to_str (fields_list);
	// eprintf ("Resulting fields json: \n%s\n", res);
	free (res);
	dso_json_dict_insert_str_key_obj (class_dict, "fields", fields_list);
	// dso_json_list_free (fields_list);
	dso_json_obj_del (fields_list);

	res = dso_json_obj_to_str (imports_list);
	// eprintf ("Resulting imports json: \n%s\n", res);
	free (res);
	dso_json_dict_insert_str_key_obj (class_dict, "imports", imports_list);
	// dso_json_list_free (imports_list);
	dso_json_obj_del (imports_list);

	// res = dso_json_obj_to_str (interfaces_list);
	// eprintf ("Resulting interfaces json: \n%s\n", res);
	// free (res);
	// dso_json_dict_insert_str_key_obj (class_dict, "interfaces", interfaces_list);

	res = dso_json_obj_to_str (class_dict);
	// eprintf ("Resulting class info json: \n%s\n", res);
	free (res);
	// dso_json_obj_del (class_dict);
	return class_dict;
}