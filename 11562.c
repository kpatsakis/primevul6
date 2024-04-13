_nc_va_tracef(const char *fmt, va_list ap)
{
    static const char Called[] = T_CALLED("");
    static const char Return[] = T_RETURN("");

    bool before = FALSE;
    bool after = FALSE;
    unsigned doit = _nc_tracing;
    int save_err = errno;
    FILE *fp = MyFP;

#ifdef TRACE
    /* verbose-trace in the command-line utilities relies on this */
    if (fp == 0 && !MyInit && _nc_tracing >= DEBUG_LEVEL(1))
	fp = stderr;
#endif

    if (strlen(fmt) >= sizeof(Called) - 1) {
	if (!strncmp(fmt, Called, sizeof(Called) - 1)) {
	    before = TRUE;
	    MyLevel++;
	} else if (!strncmp(fmt, Return, sizeof(Return) - 1)) {
	    after = TRUE;
	}
	if (before || after) {
	    if ((MyLevel <= 1)
		|| (doit & TRACE_ICALLS) != 0)
		doit &= (TRACE_CALLS | TRACE_CCALLS);
	    else
		doit = 0;
	}
    }

    if (doit != 0 && fp != 0) {
#ifdef USE_PTHREADS
	/*
	 * TRACE_ICALLS is "really" needed to show normal use with threaded
	 * applications, since anything can be running during a napms(),
	 * making it appear in the hierarchical trace as it other functions
	 * are being called.
	 *
	 * Rather than add the complication of a per-thread stack, just
	 * show the thread-id in each line of the trace.
	 */
# if USE_WEAK_SYMBOLS
	if ((pthread_self))
# endif
#ifdef _WIN32
	    fprintf(fp, "%#lx:", (long) (intptr_t) pthread_self().p);
#else
	    fprintf(fp, "%#lx:", (long) (intptr_t) pthread_self());
#endif
#endif
	if (before || after) {
	    int n;
	    for (n = 1; n < MyLevel; n++)
		fputs("+ ", fp);
	}
	vfprintf(fp, fmt, ap);
	fputc('\n', fp);
	fflush(fp);
    }

    if (after && MyLevel)
	MyLevel--;

    errno = save_err;
}