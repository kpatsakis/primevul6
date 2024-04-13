usage(void)
{
    static const char *msg[] =
    {
	"Usage: dots_termcap [options]"
	,""
	,"Options:"
	," -T TERM  override $TERM"
	," -e       allow environment $LINES / $COLUMNS"
	," -m SIZE  set margin (default: 2)"
	," -r SECS  self-interrupt/exit after specified number of seconds"
	," -s MSECS delay 1% of the time (default: 1 msecs)"
    };
    size_t n;

    for (n = 0; n < SIZEOF(msg); n++)
	fprintf(stderr, "%s\n", msg[n]);

    ExitProgram(EXIT_FAILURE);
}