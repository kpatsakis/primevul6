usage(void)
{
#define DATA(s) s "\n"
    static const char options_string[] =
    {
	DATA("Options:")
	DATA("  -0         format translation output all capabilities on one line")
	DATA("  -1         format translation output one capability per line")
#if NCURSES_XNAMES
	DATA("  -a         retain commented-out capabilities (sets -x also)")
#endif
	DATA("  -C         translate entries to termcap source form")
	DATA("  -D         print list of tic's database locations (first must be writable)")
	DATA("  -c         check only, validate input without compiling or translating")
	DATA("  -e<names>  translate/compile only entries named by comma-separated list")
	DATA("  -f         format complex strings for readability")
	DATA("  -G         format %{number} to %'char'")
	DATA("  -g         format %'char' to %{number}")
	DATA("  -I         translate entries to terminfo source form")
	DATA("  -K         translate entries to termcap source form with BSD syntax")
	DATA("  -L         translate entries to full terminfo source form")
	DATA("  -N         disable smart defaults for source translation")
	DATA("  -o<dir>    set output directory for compiled entry writes")
	DATA("  -Q[n]      dump compiled description")
	DATA("  -q    brief listing, removes headers")
	DATA("  -R<name>   restrict translation to given terminfo/termcap version")
	DATA("  -r         force resolution of all use entries in source translation")
	DATA("  -s         print summary statistics")
	DATA("  -T         remove size-restrictions on compiled description")
#if NCURSES_XNAMES
	DATA("  -t         suppress commented-out capabilities")
#endif
	DATA("  -U         suppress post-processing of entries")
	DATA("  -V         print version")
	DATA("  -W         wrap long strings according to -w[n] option")
	DATA("  -v[n]      set verbosity level")
	DATA("  -w[n]      set format width for translation output")
#if NCURSES_XNAMES
	DATA("  -x         treat unknown capabilities as user-defined")
#endif
	DATA("")
	DATA("Parameters:")
	DATA("  <file>     file to translate or compile")
    };
#undef DATA

    fprintf(stderr, "Usage: %s %s\n", _nc_progname, usage_string);
    fputs(options_string, stderr);
    ExitProgram(EXIT_FAILURE);
}