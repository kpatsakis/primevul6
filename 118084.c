warning_opts(opts, optype)
register char *opts;
const char *optype;
{
	uchar translate[MAXPCHARS+1];
	int length, i;

	if (!(opts = string_for_env_opt(optype, opts, FALSE)))
		return;
	escapes(opts, opts);

	length = strlen(opts);
	if (length > WARNCOUNT) length = WARNCOUNT;
	/* match the form obtained from PC configuration files */
	for (i = 0; i < length; i++)
	     translate[i] = (((i < WARNCOUNT) && opts[i]) ?
			   (uchar) opts[i] : def_warnsyms[i].sym);
	assign_warnings(translate);
}