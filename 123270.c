static ut32 r_cmd_get_num_classname_str_occ (const char * str, const char *match_me) {
	const char *result = NULL;
	ut32 len = match_me && *match_me ? strlen (match_me) : 0;
	ut32 occ = 0;

	if (len == 0 || !str || !*str) {
		return 0;
	}
	result = str;
	while (result && *result && (result - str < len)) {
		result = strstr (result, match_me);
		if (result) {
			IFDBG eprintf ("result: %s\n", result);
			result+=len;
			occ++;
		}
	}
	return occ;
}