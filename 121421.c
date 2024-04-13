TrackDown(XtermWidget xw, XButtonEvent *event)
{
    TScreen *screen = TScreenOf(xw);
    CELL cell;

    screen->selectUnit = EvalSelectUnit(xw,
					event->time,
					Select_CHAR,
					event->button);
    if (screen->numberOfClicks > 1) {
	PointToCELL(screen, event->y, event->x, &cell);
	screen->replyToEmacs = True;
	StartSelect(xw, &cell);
    } else {
	screen->waitingForTrackInfo = True;
	EditorButton(xw, event);
    }
}