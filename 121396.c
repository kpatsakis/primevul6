HandleKeyboardStartExtend(Widget w,
			  XEvent *event,	/* must be XButtonEvent* */
			  String *params,	/* unused */
			  Cardinal *num_params)		/* unused */
{
    XtermWidget xw;

    if ((xw = getXtermWidget(w)) != 0) {
	TRACE_EVENT("HandleKeyboardStartExtend", event, params, num_params);
	do_start_extend(xw, event, params, num_params, True);
    }
}