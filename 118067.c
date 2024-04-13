string_for_env_opt(optname, opts, val_optional)
const char *optname;
char *opts;
boolean val_optional;
{
	if(!initial) {
		rejectoption(optname);
		return (char *)0;
	}
	return string_for_opt(opts, val_optional);
}