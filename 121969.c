R_API DsoJsonObj *r_bin_java_get_method_json_definitions(RBinJavaObj *bin) {
	RBinJavaField *fm_type = NULL;
	RListIter *iter = NULL;
	DsoJsonObj *json_list = dso_json_list_new ();
	if (!bin) {
		return json_list;
	}
	r_list_foreach (bin->methods_list, iter, fm_type) {
		DsoJsonObj *method_proto = r_bin_java_get_method_json_definition (bin, fm_type);
		// eprintf ("Method json: %s\n", method_proto);
		dso_json_list_append (json_list, method_proto);
	}
	return json_list;
}