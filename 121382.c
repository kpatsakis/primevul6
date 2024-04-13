HandleExecSelectable(Widget w,
		     XEvent *event,
		     String *params,	/* selections */
		     Cardinal *num_params)
{
    XtermWidget xw;

    if ((xw = getXtermWidget(w)) != 0) {
	TRACE_EVENT("HandleExecSelectable", event, params, num_params);

	if (*num_params == 2) {
	    CELL start, finish;
	    char *data;
	    char **argv;

	    data = getDataFromScreen(xw, event, params[1], &start, &finish);
	    if (data != 0) {
		if ((argv = tokenizeFormat(params[0])) != 0) {
		    char *blob = argv[0];
		    int argc;

		    for (argc = 0; argv[argc] != 0; ++argc) {
			argv[argc] = expandFormat(xw, argv[argc], data,
						  &start, &finish);
		    }
		    executeCommand(TScreenOf(xw)->pid, argv);
		    freeArgv(blob, argv);
		}
		free(data);
	    }
	}
    }
}