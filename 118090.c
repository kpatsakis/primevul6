complain_about_duplicate(opts, iscompound)
const char *opts;
int iscompound;		/* 0 == boolean option, 1 == compound */
{
#if defined(MAC)
	/* the Mac has trouble dealing with the output of messages while
	 * processing the config file.  That should get fixed one day.
	 * For now just return.
	 */
	return;
#endif

	raw_printf(
		"\nWarning - %s option specified multiple times: %s.\n",
		iscompound ? "compound" : "boolean", opts);
	wait_synch();
	return;
}