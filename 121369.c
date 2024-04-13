reallyExecFormatted(Widget w, char *format, char *data, CELL *start, CELL *finish)
{
    XtermWidget xw;

    if ((xw = getXtermWidget(w)) != 0) {
	char **argv;

	if ((argv = tokenizeFormat(format)) != 0) {
	    char *blob = argv[0];
	    int argc;

	    for (argc = 0; argv[argc] != 0; ++argc) {
		argv[argc] = expandFormat(xw, argv[argc], data, start, finish);
	    }
	    executeCommand(TScreenOf(xw)->pid, argv);
	    freeArgv(blob, argv);
	}
    }
}