static char *scanarg(char *s, char del)
{
	char c;

	while ((c = *s++) != del) {
		if (c == '\\' && *s == 'x') {
			s++;
			if (!isxdigit(*s++))
				return NULL;
			if (!isxdigit(*s++))
				return NULL;
		}
	}
	return s;
}