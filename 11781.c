usage(void)
{
    static const char *msg[] =
    {
	"Usage: test_tparm [options] [capability] [value1 [value2 [...]]]",
	"",
	"Print all distinct combinations of given capability.",
	"",
	"Options:",
	" -T TERM  override $TERM; this may be a comma-separated list or \"-\"",
	"          to read a list from standard-input",
	" -a       if capability is given, test all combinations of values",
	" -r NUM   repeat tests NUM times",
	" -v       show values and results",
    };
    unsigned n;
    for (n = 0; n < SIZEOF(msg); ++n) {
	fprintf(stderr, "%s\n", msg[n]);
    }
    ExitProgram(EXIT_FAILURE);
}