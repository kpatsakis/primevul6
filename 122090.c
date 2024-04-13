R_API DsoJsonObj *r_bin_java_get_field_json_definition(RBinJavaObj *bin, RBinJavaField *fm_type) {
	return r_bin_java_get_fm_type_definition_json (bin, fm_type, 0);
}