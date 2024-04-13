HandleSelectExtend(Widget w,
		   XEvent *event,	/* must be XMotionEvent */
		   String *params GCC_UNUSED,
		   Cardinal *num_params GCC_UNUSED)
{
    XtermWidget xw;

    if ((xw = getXtermWidget(w)) != 0) {
	TScreen *screen = TScreenOf(xw);
	CELL cell;

	TRACE_EVENT("HandleSelectExtend", event, params, num_params);

	screen->selection_time = event->xmotion.time;
	switch (screen->eventMode) {
	    /* If not in one of the DEC mouse-reporting modes */
	case LEFTEXTENSION:
	case RIGHTEXTENSION:
	    PointToCELL(screen, event->xmotion.y, event->xmotion.x, &cell);
	    ExtendExtend(xw, &cell);
	    break;

	    /* If in motion reporting mode, send mouse position to
	       character process as a key sequence \E[M... */
	case NORMAL:
	    /* will get here if send_mouse_pos != MOUSE_OFF */
	    if (okSendMousePos(xw) == BTN_EVENT_MOUSE
		|| okSendMousePos(xw) == ANY_EVENT_MOUSE) {
		(void) SendMousePosition(xw, event);
	    }
	    break;
	}
    }
}