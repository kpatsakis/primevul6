HandleKeyboardSelectEnd(Widget w,
			XEvent *event,	/* must be XButtonEvent */
			String *params,		/* selections */
			Cardinal *num_params)
{
    XtermWidget xw;

    if ((xw = getXtermWidget(w)) != 0) {
	TRACE(("HandleKeyboardSelectEnd\n"));
	do_select_end(xw, event, params, num_params, True);
    }
}