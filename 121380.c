HandleKeyboardSelectExtend(Widget w,
			   XEvent *event GCC_UNUSED,	/* must be XButtonEvent */
			   String *params GCC_UNUSED,
			   Cardinal *num_params GCC_UNUSED)
{
    XtermWidget xw;

    if ((xw = getXtermWidget(w)) != 0) {
	TScreen *screen = TScreenOf(xw);

	TRACE_EVENT("HandleKeyboardSelectExtend", event, params, num_params);
	ExtendExtend(xw, &screen->cursorp);
    }
}