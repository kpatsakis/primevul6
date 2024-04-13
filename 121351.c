do_start_extend(XtermWidget xw,
		XEvent *event,	/* must be XButtonEvent* */
		String *params GCC_UNUSED,
		Cardinal *num_params GCC_UNUSED,
		Bool use_cursor_loc)
{
    TScreen *screen = TScreenOf(xw);
    int coord;
    CELL cell;

    if (SendMousePosition(xw, event))
	return;

    screen->firstValidRow = 0;
    screen->lastValidRow = screen->max_row;
#if OPT_READLINE
    if (OverrideEvent(event)
	|| event->xbutton.button != Button3
	|| !(SCREEN_FLAG(screen, dclick3_deletes)))
#endif
	screen->selectUnit = EvalSelectUnit(xw,
					    event->xbutton.time,
					    screen->selectUnit,
					    event->xbutton.button);
    screen->replyToEmacs = False;

#if OPT_READLINE
    CheckSecondPress3(xw, screen, event);
#endif

    if (screen->numberOfClicks == 1
	|| (SCREEN_FLAG(screen, dclick3_deletes)
	    && !OverrideEvent(event))) {
	/* Save existing selection so we can reestablish it if the guy
	   extends past the other end of the selection */
	screen->saveStartR = screen->startExt = screen->startRaw;
	screen->saveEndR = screen->endExt = screen->endRaw;
    } else {
	/* He just needed the selection mode changed, use old values. */
	screen->startExt = screen->startRaw = screen->saveStartR;
	screen->endExt = screen->endRaw = screen->saveEndR;
    }
    if (use_cursor_loc) {
	cell = screen->cursorp;
    } else {
	PointToCELL(screen, event->xbutton.y, event->xbutton.x, &cell);
    }
    coord = Coordinate(screen, &cell);

    if (Abs(coord - Coordinate(screen, &(screen->startSel)))
	< Abs(coord - Coordinate(screen, &(screen->endSel)))
	|| coord < Coordinate(screen, &(screen->startSel))) {
	/* point is close to left side of selection */
	screen->eventMode = LEFTEXTENSION;
	screen->startExt = cell;
    } else {
	/* point is close to left side of selection */
	screen->eventMode = RIGHTEXTENSION;
	screen->endExt = cell;
    }
    ComputeSelect(xw, &(screen->startExt), &(screen->endExt), True, True);

#if OPT_READLINE
    if (!isSameCELL(&(screen->startSel), &(screen->endSel)))
	ExtendingSelection = 1;
#endif
}