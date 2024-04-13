parsesymbols(opts)
register char *opts;
{
	int val;
	char *op, *symname, *strval, *p;
	struct symparse *symp;

	if ((op = index(opts, ',')) != 0) {
		*op++ = 0;
		parsesymbols(op);
	}

	/* S_sample:string */
	symname = opts;
	strval = index(opts, ':');
	if (!symname || !strval) return;
	*strval++ = 0;

	/* strip leading and trailing white space from symname */
	while (isspace(*symname)) symname++;
	p = eos(symname);
	while (--p >= symname && isspace(*p)) *p = '\0';

	/* strip leading and trailing white space from strval */
	while (isspace(*strval)) strval++;
	p = eos(strval);
	while (--p >= strval && isspace(*p)) *p = '\0';

	symp = match_sym(symname);
	if (!symp) return;

	if (symp->range && symp->range != SYM_CONTROL) {
		val = sym_val(strval);
		update_l_symset(symp, val);
	}
}