HandlePointerMotion(Widget w,
		    XEvent *event,
		    String *params,	/* selections */
		    Cardinal *num_params)
{
    XtermWidget xw;

    (void) params;
    (void) num_params;
    if ((xw = getXtermWidget(w)) != 0) {
	TRACE(("HandlePointerMotion\n"));
	if (event->type == MotionNotify)
	    (void) SendMousePosition(xw, event);
    }
}