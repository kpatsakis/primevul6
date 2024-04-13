usage(void)
{
#define DATA(s) s "\n"
    static const char msg[] =
    {
	DATA("Usage: tabs [options] [tabstop-list]")
	DATA("")
	DATA("Options:")
	DATA("  -0       reset tabs")
	DATA("  -8       set tabs to standard interval")
	DATA("  -a       Assembler, IBM S/370, first format")
	DATA("  -a2      Assembler, IBM S/370, second format")
	DATA("  -c       COBOL, normal format")
	DATA("  -c2      COBOL compact format")
	DATA("  -c3      COBOL compact format extended")
	DATA("  -d       debug (show ruler with expected/actual tab positions)")
	DATA("  -f       FORTRAN")
	DATA("  -n       no-op (do not modify terminal settings)")
	DATA("  -p       PL/I")
	DATA("  -s       SNOBOL")
	DATA("  -u       UNIVAC 1100 Assembler")
	DATA("  -T name  use terminal type 'name'")
	DATA("  -V       print version")
	DATA("")
	DATA("A tabstop-list is an ordered list of column numbers, e.g., 1,11,21")
	DATA("or 1,+10,+10 which is the same.")
    };
#undef DATA

    fflush(stdout);
    fputs(msg, stderr);
    ExitProgram(EXIT_FAILURE);
}