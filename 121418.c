doSelectionFormat(XtermWidget xw,
		  Widget w,
		  XEvent *event,
		  String *params,
		  Cardinal *num_params,
		  FormatSelect format_select)
{
    TScreen *screen = TScreenOf(xw);
    InternalSelect *mydata = &(screen->internal_select);

    memset(mydata, 0, sizeof(*mydata));
    mydata->format = x_strdup(params[0]);
    mydata->format_select = format_select;

    screen->selectToBuffer = True;
    beginInternalSelect(xw);

    xtermGetSelection(w, getEventTime(event), params + 1, *num_params - 1, NULL);

    if (screen->selectToBuffer)
	finishInternalSelect(xw);
}