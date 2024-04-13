HandleKeyboardSelectStart(Widget w,
			  XEvent *event,	/* must be XButtonEvent* */
			  String *params GCC_UNUSED,
			  Cardinal *num_params GCC_UNUSED)
{
    XtermWidget xw;

    if ((xw = getXtermWidget(w)) != 0) {
	TScreen *screen = TScreenOf(xw);

	TRACE_EVENT("HandleKeyboardSelectStart", event, params, num_params);
	do_select_start(xw, event, &screen->cursorp);
    }
}