static char *getrange(char *s) {
	char *p = NULL;
	while (s && *s) {
		if (*s == ',') {
			p = s+1;
			*p=0;
		}
		if (*s == '[' || *s == ']') {
			memmove (s, s + 1, strlen (s + 1) + 1);
		}
		if (*s == '}') {
			*s = 0;
		}
		s++;
	}
	while (p && *p == ' ') {
		p++;
	}
	return p;
}