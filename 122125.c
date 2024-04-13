R_API char *r_bin_java_unmangle_method(const char *flags, const char *name, const char *params, const char *r_value) {
	RList *the_list = params ? r_bin_java_extract_type_values (params) : r_list_new ();
	RListIter *iter = NULL;
	// second case removes leading space if no flags are given
	const char *fmt = flags ? "%s %s %s (%s)" : "%s%s %s (%s)";
	char *str = NULL, *f_val_str = NULL, *r_val_str = NULL, *prototype = NULL, *p_val_str = NULL;
	ut32 params_idx = 0, params_len = 0, prototype_len = 0;
	if (!extract_type_value (r_value, &r_val_str)) {
		r_list_free (the_list);
		return NULL;
	}
	if (!r_val_str) {
		r_val_str = strdup ("UNKNOWN");
	}
	f_val_str = strdup (flags ? flags : "");
	params_idx = 0;
	r_list_foreach (the_list, iter, str) {
		params_len += strlen (str);
		if (params_idx > 0) {
			params_len += 2;
		}
		params_idx++;
	}
	if (params_len > 0) {
		ut32 offset = 0;
		params_len += 1;
		p_val_str = malloc (params_len);
		params_idx = 0;
		r_list_foreach (the_list, iter, str) {
			if (offset != 0) {
				offset += snprintf (p_val_str + offset, params_len - offset, ", %s", str);
			} else {
				offset += snprintf (p_val_str + offset, params_len - offset, "%s", str);
			}
		}
	} else {
		p_val_str = strdup ("");
	}

	prototype_len += (flags ? strlen (flags) + 1 : 0); // space vs no space
	prototype_len += strlen (name) + 1; // name + space
	prototype_len += strlen (r_val_str) + 1; // r_value + space
	prototype_len += strlen (p_val_str) + 3; // space + l_paren + params + r_paren
	prototype_len += 1; // null
	prototype = malloc (prototype_len);
	/// TODO enable this function and start using it to demangle strings
	snprintf (prototype, prototype_len, fmt, f_val_str, r_val_str, name, p_val_str);
	free (f_val_str);
	free (r_val_str);
	free (p_val_str);
	r_list_free (the_list);
	return prototype;
}