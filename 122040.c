R_API DsoJsonObj *r_bin_java_get_field_json_definitions(RBinJavaObj *bin) {
	RBinJavaField *fm_type = NULL;
	RListIter *iter = NULL;
	DsoJsonObj *json_list = dso_json_list_new ();
	if (!bin) {
		return json_list;
	}
	r_list_foreach (bin->fields_list, iter, fm_type) {
		DsoJsonObj *field_proto = r_bin_java_get_field_json_definition (bin, fm_type);
		// eprintf ("Field json: %s\n", field_proto);
		dso_json_list_append (json_list, field_proto);
	}
	return json_list;
}