skip_wdvarname(const char *s,
    /* skip array de-reference? */
    bool aok)
{
	if (s[0] == CHAR && ksh_isalphx(s[1])) {
		do {
			s += 2;
		} while (s[0] == CHAR && ksh_isalnux(s[1]));
		if (aok && s[0] == CHAR && s[1] == '[') {
			/* skip possible array de-reference */
			const char *p = s;
			char c;
			int depth = 0;

			while (/* CONSTCOND */ 1) {
				if (p[0] != CHAR)
					break;
				c = p[1];
				p += 2;
				if (c == '[')
					depth++;
				else if (c == ']' && --depth == 0) {
					s = p;
					break;
				}
			}
		}
	}
	return (s);
}