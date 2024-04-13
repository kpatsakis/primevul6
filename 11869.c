usage(void)
{
    static const char *msg[] =
    {
	"Usage: dots_xcurses [options]"
	,""
	,"Options:"
	," -T TERM  override $TERM"
#if HAVE_USE_DEFAULT_COLORS
	," -d       invoke use_default_colors()"
#endif
#if HAVE_USE_ENV
	," -e       allow environment $LINES / $COLUMNS"
#endif
	," -m SIZE  set margin (default: 2)"
	," -r SECS  self-interrupt/exit after specified number of seconds"
	," -s MSECS delay 1% of the time (default: 1 msecs)"
#if HAVE_ALLOC_PAIR
	," -x       use alloc_pair() rather than init_pair()"
#endif
    };
    size_t n;

    for (n = 0; n < SIZEOF(msg); n++)
	fprintf(stderr, "%s\n", msg[n]);

    ExitProgram(EXIT_FAILURE);
}