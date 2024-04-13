is_wdvarassign(const char *s)
{
	const char *p = skip_wdvarname(s, true);

	return (p != s && p[0] == CHAR &&
	    (p[1] == '=' || (p[1] == '+' && p[2] == CHAR && p[3] == '=')));
}