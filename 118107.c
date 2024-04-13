rejectoption(optname)
const char *optname;
{
#ifdef MICRO
	pline("\"%s\" settable only from %s.", optname, configfile);
#else
	pline("%s can be set only from NETHACKOPTIONS or %s.", optname,
			configfile);
#endif
}