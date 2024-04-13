immedhook(ENTRY * ep GCC_UNUSED)
/* write out entries with no use capabilities immediately to save storage */
{
#if !HAVE_BIG_CORE
    /*
     * This is strictly a core-economy kluge.  The really clean way to handle
     * compilation is to slurp the whole file into core and then do all the
     * name-collision checks and entry writes in one swell foop.  But the
     * terminfo master file is large enough that some core-poor systems swap
     * like crazy when you compile it this way...there have been reports of
     * this process taking *three hours*, rather than the twenty seconds or
     * less typical on my development box.
     *
     * So.  This hook *immediately* writes out the referenced entry if it
     * has no use capabilities.  The compiler main loop refrains from
     * adding the entry to the in-core list when this hook fires.  If some
     * other entry later needs to reference an entry that got written
     * immediately, that's OK; the resolution code will fetch it off disk
     * when it can't find it in core.
     *
     * Name collisions will still be detected, just not as cleanly.  The
     * write_entry() code complains before overwriting an entry that
     * postdates the time of tic's first call to write_entry().  Thus
     * it will complain about overwriting entries newly made during the
     * tic run, but not about overwriting ones that predate it.
     *
     * The reason this is a hook, and not in line with the rest of the
     * compiler code, is that the support for termcap fallback cannot assume
     * it has anywhere to spool out these entries!
     *
     * The _nc_set_type() call here requires a compensating one in
     * _nc_parse_entry().
     *
     * If you define HAVE_BIG_CORE, you'll disable this kluge.  This will
     * make tic a bit faster (because the resolution code won't have to do
     * disk I/O nearly as often).
     */
    if (ep->nuses == 0) {
	int oldline = _nc_curr_line;

	write_it(ep);
	_nc_curr_line = oldline;
	free(ep->tterm.str_table);
	return (TRUE);
    }
#endif /* HAVE_BIG_CORE */
    return (FALSE);
}