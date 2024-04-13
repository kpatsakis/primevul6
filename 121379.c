HandlePointerButton(Widget w,
		    XEvent *event,
		    String *params,	/* selections */
		    Cardinal *num_params)
{
    XtermWidget xw;

    (void) params;
    (void) num_params;
    if ((xw = getXtermWidget(w)) != 0) {
	TRACE(("HandlePointerButton\n"));
	if (IsBtnEvent(event))
	    (void) SendMousePosition(xw, event);
    }
}