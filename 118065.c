badoption(opts)
const char *opts;
{
	if (!initial) {
	    if (!strncmp(opts, "h", 1) || !strncmp(opts, "?", 1))
		option_help();
	    else
		pline("Bad syntax: %s.  Enter \"?g\" for help.", opts);
	    return;
	}
#ifdef MAC
	else return;
#endif

	if(from_file)
	    raw_printf("Bad syntax in OPTIONS in %s: %s.", configfile, opts);
	else
	    raw_printf("Bad syntax in NETHACKOPTIONS: %s.", opts);

	wait_synch();
}