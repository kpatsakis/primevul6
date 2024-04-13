match_sym(buf)
char *buf;
{
	size_t len = strlen(buf);
	const char *p = index(buf, ':'),
		   *q = index(buf, '=');
	struct symparse *sp = loadsyms;

	if (!p || (q && q < p)) p = q;
	while(p && p > buf && isspace(*(p-1))) p--;
	if (p) len = (int)(p - buf);
	while(sp->range) {
	    if ((len >= strlen(sp->name)) && !strncmpi(buf, sp->name, len))
		return sp;
	    sp++;
	}
	return (struct symparse *)0;
}