HandleInsertSelectable(Widget w,
		       XEvent *event,
		       String *params,	/* selections */
		       Cardinal *num_params)
{
    XtermWidget xw;

    if ((xw = getXtermWidget(w)) != 0) {
	TRACE_EVENT("HandleInsertSelectable", event, params, num_params);

	if (*num_params == 2) {
	    CELL start, finish;
	    char *data;
	    char *temp = x_strdup(params[0]);

	    data = getDataFromScreen(xw, event, params[1], &start, &finish);
	    if (data != 0) {
		char *exps = expandFormat(xw, temp, data, &start, &finish);
		if (exps != 0) {
		    unparseputs(xw, exps);
		    unparse_end(xw);
		    free(exps);
		}
		free(data);
	    }
	    free(temp);
	}
    }
}