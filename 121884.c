R_API ut16 r_bin_java_calculate_class_access_value(const char *access_flags_str) {
	return calculate_access_value (access_flags_str, CLASS_ACCESS_FLAGS);
}