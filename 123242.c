static const char * r_cmd_java_strtok (const char *str1, const char b, size_t len) {
	const char *p = str1;
	size_t i = 0;
	if (!p || !*p) {
		return p;
	}
	if (len == -1) {
		len = strlen (str1);
	}
	IFDBG r_cons_printf ("Looking for char (%c) in (%s) up to %d\n", b, p, len);
	for ( ; i < len; i++,p++) {
		if (*p == b) {
			IFDBG r_cons_printf ("Found? for char (%c) @ %d: (%s)\n", b, i, p);
			break;
		}
	}
	if (i == len) {
		p = NULL;
	}
	IFDBG r_cons_printf ("Found? for char (%c) @ %d: (%s)\n", b, len, p);
	return p;
}