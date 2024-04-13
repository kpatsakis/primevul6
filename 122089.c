R_API char *r_bin_java_unmangle(const char *flags, const char *name, const char *descriptor) {
	ut32 l_paren_pos = -1, r_paren_pos = -1;
	char *result = NULL;
	ut32 desc_len = descriptor && *descriptor ? strlen (descriptor) : 0,
	name_len = name && *name ? strlen (name) : 0,
	flags_len = flags && *flags ? strlen (flags) : 0,
	i = 0;
	if (desc_len == 0 || name == 0) {
		return NULL;
	}
	for (i = 0; i < desc_len; i++) {
		if (descriptor[i] == '(') {
			l_paren_pos = i;
		} else if (l_paren_pos != (ut32) - 1 && descriptor[i] == ')') {
			r_paren_pos = i;
			break;
		}
	}
	// handle field case;
	if (l_paren_pos == (ut32) - 1 && r_paren_pos == (ut32) - 1) {
		char *unmangle_field_desc = NULL;
		ut32 len = extract_type_value (descriptor, &unmangle_field_desc);
		if (len == 0) {
			eprintf ("Warning: attempting to unmangle invalid type descriptor.\n");
			free (unmangle_field_desc);
			return result;
		}
		if (flags_len > 0) {
			len += (flags_len + name_len + 5); // space and null
			result = malloc (len);
			snprintf (result, len, "%s %s %s", flags, unmangle_field_desc, name);
		} else {
			len += (name_len + 5); // space and null
			result = malloc (len);
			snprintf (result, len, "%s %s", unmangle_field_desc, name);
		}
		free (unmangle_field_desc);
	} else if (l_paren_pos != (ut32) - 1 &&
	r_paren_pos != (ut32) - 1 &&
	l_paren_pos < r_paren_pos) {
		// params_len account for l_paren + 1 and null
		ut32 params_len = r_paren_pos - (l_paren_pos + 1) != 0 ? r_paren_pos - (l_paren_pos + 1) + 1 : 0;
		char *params = params_len ? malloc (params_len) : NULL;
		const char *rvalue = descriptor + r_paren_pos + 1;
		if (params) {
			snprintf (params, params_len, "%s", descriptor + l_paren_pos + 1);
		}
		result = r_bin_java_unmangle_method (flags, name, params, rvalue);
		free (params);
	}
	return result;
}