do_select_end(XtermWidget xw,
	      XEvent *event,	/* must be XButtonEvent */
	      String *params,	/* selections */
	      Cardinal *num_params,
	      Bool use_cursor_loc)
{
    TScreen *screen = TScreenOf(xw);

    screen->selection_time = event->xbutton.time;

    TRACE(("do_select_end %s @%ld\n",
	   visibleEventMode(screen->eventMode),
	   screen->selection_time));

    switch (screen->eventMode) {
    case NORMAL:
	(void) SendMousePosition(xw, event);
	break;
    case LEFTEXTENSION:
    case RIGHTEXTENSION:
	EndExtend(xw, event, params, *num_params, use_cursor_loc);
#if OPT_READLINE
	readlineExtend(xw, event);
#endif /* OPT_READLINE */
	break;
    }
}