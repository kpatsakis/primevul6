static const char * r_cmd_java_consumetok (const char *str1, const char b, size_t len) {
	const char *p = str1;
	size_t i = 0;
	if (!p) {
		return p;
	}
	if (len == -1) {
		len = strlen (str1);
	}
	for ( ; i < len; i++,p++) {
		if (*p != b) {
			break;
		}
	}
	return p;
}