static const char * r_cmd_get_next_classname_str (const char * str, const char *match_me) {
	const char *result = NULL;
	ut32 len = match_me && *match_me ? strlen (match_me) : 0;
	if (len && str && *str) {
		result = str;
		while (result && *result && (result - str < len)) {
			result = strstr (result, match_me);
			if (result) {
				break;
			}
		}
	}
	return result;
}