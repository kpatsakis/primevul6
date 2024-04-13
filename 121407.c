HandleSelectSet(Widget w,
		XEvent *event,
		String *params,
		Cardinal *num_params)
{
    XtermWidget xw;

    if ((xw = getXtermWidget(w)) != 0) {
	TRACE_EVENT("HandleSelectSet", event, params, num_params);
	SelectSet(xw, event, params, *num_params);
    }
}