HandleCopySelection(Widget w,
		    XEvent *event,
		    String *params,	/* list of targets */
		    Cardinal *num_params)
{
    XtermWidget xw;

    if ((xw = getXtermWidget(w)) != 0) {
	TRACE_EVENT("HandleCopySelection", event, params, num_params);
	SelectSet(xw, event, params, *num_params);
    }
}