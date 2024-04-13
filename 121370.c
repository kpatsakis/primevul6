StartSelect(XtermWidget xw, const CELL *cell)
{
    TScreen *screen = TScreenOf(xw);

    TRACE(("StartSelect row=%d, col=%d\n", cell->row, cell->col));
    if (screen->cursor_state)
	HideCursor(xw);
    if (screen->numberOfClicks == 1) {
	/* set start of selection */
	screen->rawPos = *cell;
    }
    /* else use old values in rawPos */
    screen->saveStartR = screen->startExt = screen->rawPos;
    screen->saveEndR = screen->endExt = screen->rawPos;
    if (Coordinate(screen, cell) < Coordinate(screen, &(screen->rawPos))) {
	screen->eventMode = LEFTEXTENSION;
	screen->startExt = *cell;
    } else {
	screen->eventMode = RIGHTEXTENSION;
	screen->endExt = *cell;
    }
    ComputeSelect(xw, &(screen->startExt), &(screen->endExt), False, True);
}