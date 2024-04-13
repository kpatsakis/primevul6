ResizeSelection(TScreen *screen, int rows, int cols)
{
    rows--;			/* decr to get 0-max */
    cols--;

    if (screen->startRaw.row > rows)
	screen->startRaw.row = rows;
    if (screen->startSel.row > rows)
	screen->startSel.row = rows;
    if (screen->endRaw.row > rows)
	screen->endRaw.row = rows;
    if (screen->endSel.row > rows)
	screen->endSel.row = rows;
    if (screen->rawPos.row > rows)
	screen->rawPos.row = rows;

    if (screen->startRaw.col > cols)
	screen->startRaw.col = cols;
    if (screen->startSel.col > cols)
	screen->startSel.col = cols;
    if (screen->endRaw.col > cols)
	screen->endRaw.col = cols;
    if (screen->endSel.col > cols)
	screen->endSel.col = cols;
    if (screen->rawPos.col > cols)
	screen->rawPos.col = cols;
}