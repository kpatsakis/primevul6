readlineExtend(XtermWidget xw, XEvent *event)
{
    TScreen *screen = TScreenOf(xw);
    int ldelta1, ldelta2;

    if (IsBtnEvent(event)) {
	XButtonEvent *my_event = (XButtonEvent *) event;
	if (isClick1_clean(xw, my_event)
	    && SCREEN_FLAG(screen, click1_moves)
	    && rowOnCurrentLine(screen, eventRow(screen, event), &ldelta1)) {
	    ReadLineMovePoint(screen, eventColBetween(screen, event), ldelta1);
	}
	if (isDoubleClick3(xw, screen, my_event)
	    && SCREEN_FLAG(screen, dclick3_deletes)
	    && rowOnCurrentLine(screen, screen->startSel.row, &ldelta1)
	    && rowOnCurrentLine(screen, screen->endSel.row, &ldelta2)) {
	    ReadLineMovePoint(screen, screen->endSel.col, ldelta2);
	    ReadLineDelete(screen, &screen->startSel, &(screen->endSel));
	}
    }
}