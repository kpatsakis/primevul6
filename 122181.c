R_API void U(r_bin_java_free_const_value)(ConstJavaValue * cp_value) {
	char first_char = cp_value && cp_value->type ? *cp_value->type : 0,
	second_char = cp_value && cp_value->type ? *(cp_value->type + 1) : 0;
	switch (first_char) {
	case 'r':
		if (cp_value && cp_value->value._ref) {
			free (cp_value->value._ref->class_name);
			free (cp_value->value._ref->name);
			free (cp_value->value._ref->desc);
		}
		break;
	case 's':
		if (second_char == 't' && cp_value->value._str) {
			free (cp_value->value._str->str);
		}
		break;
	}
	free (cp_value);
}