HandleSelectStart(Widget w,
		  XEvent *event,	/* must be XButtonEvent* */
		  String *params GCC_UNUSED,
		  Cardinal *num_params GCC_UNUSED)
{
    XtermWidget xw;

    if ((xw = getXtermWidget(w)) != 0) {
	TScreen *screen = TScreenOf(xw);
	CELL cell;

	TRACE_EVENT("HandleSelectStart", event, params, num_params);
	screen->firstValidRow = 0;
	screen->lastValidRow = screen->max_row;
	PointToCELL(screen, event->xbutton.y, event->xbutton.x, &cell);

#if OPT_READLINE
	ExtendingSelection = 0;
#endif

	do_select_start(xw, event, &cell);
    }
}