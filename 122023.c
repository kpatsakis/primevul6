R_API int r_bin_java_is_fm_type_protected(RBinJavaField *fm_type) {
	if (fm_type && fm_type->type == R_BIN_JAVA_FIELD_TYPE_METHOD) {
		return fm_type->flags & R_BIN_JAVA_METHOD_ACC_PROTECTED;
	}
	if (fm_type && fm_type->type == R_BIN_JAVA_FIELD_TYPE_FIELD) {
		return fm_type->flags & R_BIN_JAVA_FIELD_ACC_PROTECTED;
	}
	return 0;
}