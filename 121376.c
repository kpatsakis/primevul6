HandleInsertSelection(Widget w,
		      XEvent *event,	/* assumed to be XButtonEvent* */
		      String *params,	/* selections in precedence order */
		      Cardinal *num_params)
{
    XtermWidget xw;

    if ((xw = getXtermWidget(w)) != 0) {
	TRACE_EVENT("HandleInsertSelection", event, params, num_params);
	if (!SendMousePosition(xw, event)) {
#if OPT_READLINE
	    int ldelta;
	    TScreen *screen = TScreenOf(xw);
	    if (IsBtnEvent(event)
		&& !OverrideEvent(event)
		&& (okSendMousePos(xw) == MOUSE_OFF)
		&& SCREEN_FLAG(screen, paste_moves)
		&& rowOnCurrentLine(screen, eventRow(screen, event), &ldelta))
		ReadLineMovePoint(screen, eventColBetween(screen, event), ldelta);
#endif /* OPT_READLINE */

	    xtermGetSelection(w, event->xbutton.time, params, *num_params, NULL);
	}
    }
}