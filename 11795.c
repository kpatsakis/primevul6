usage(void)
{
#define KEEP(s) s "\n"
    static const char msg[] =
    {
	KEEP("")
	KEEP("Options:")
	KEEP("  -S <<       read commands from standard input")
	KEEP("  -T TERM     use this instead of $TERM")
	KEEP("  -V          print curses-version")
	KEEP("  -x          do not try to clear scrollback")
	KEEP("")
	KEEP("Commands:")
	KEEP("  clear       clear the screen")
	KEEP("  init        initialize the terminal")
	KEEP("  reset       reinitialize the terminal")
	KEEP("  capname     unlike clear/init/reset, print value for capability \"capname\"")
    };
#undef KEEP
    (void) fprintf(stderr, "Usage: %s [options] [command]\n", prg_name);
    fputs(msg, stderr);
    ExitProgram(ErrUsage);
}