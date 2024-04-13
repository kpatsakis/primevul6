R_API int extract_type_value(const char *arg_str, char **output) {
	ut8 found_one = 0, array_cnt = 0;
	ut32 len = 0, consumed = 0;
	char *str = NULL;
	if (!arg_str || !output) {
		return 0;
	}
	if (output && *output && *output != NULL) {
		free (*output);
		*output = NULL;
	}
	while (arg_str && *arg_str && !found_one) {
		len = 0;
		// handle the end of an object
		switch (*arg_str) {
		case 'V':
			len = 1;
			str = get_type_value_str ("void", array_cnt);
			break;
		case 'J':
			len = 1;
			str = get_type_value_str ("long", array_cnt);
			array_cnt = 0;
			break;
		case 'I':
			len = 1;
			str = get_type_value_str ("int", array_cnt);
			array_cnt = 0;
			break;
		case 'D':
			len = 1;
			str = get_type_value_str ("double", array_cnt);
			array_cnt = 0;
			break;
		case 'F':
			len = 1;
			str = get_type_value_str ("float", array_cnt);
			array_cnt = 0;
			break;
		case 'B':
			len = 1;
			str = get_type_value_str ("byte", array_cnt);
			array_cnt = 0;
			break;
		case 'C':
			len = 1;
			str = get_type_value_str ("char", array_cnt);
			array_cnt = 0;
			break;
		case 'Z':
			len = 1;
			str = get_type_value_str ("boolean", array_cnt);
			array_cnt = 0;
			break;
		case 'S':
			len = 1;
			str = get_type_value_str ("short", array_cnt);
			array_cnt = 0;
			break;
		case '[': len = 1; array_cnt++; break;
		case 'L':
			len = r_bin_java_extract_reference_name (arg_str, &str, array_cnt);
			array_cnt = 0;
			break;
		case '(': len = 1; str = strdup ("("); break;
		case ')': len = 1; str = strdup (")"); break;
		default:
			return 0;
		}
		if (len < 1) {
			break;
		}
		consumed += len;
		arg_str += len;
		if (str) {
			*output = str;
			break;
		}
	}
	return consumed;
}