getDataFromScreen(XtermWidget xw, XEvent *event, String method, CELL *start, CELL *finish)
{
    TScreen *screen = TScreenOf(xw);

    CELL save_old_start = screen->startH;
    CELL save_old_end = screen->endH;

    CELL save_startSel = screen->startSel;
    CELL save_startRaw = screen->startRaw;
    CELL save_finishSel = screen->endSel;
    CELL save_finishRaw = screen->endRaw;

    int save_firstValidRow = screen->firstValidRow;
    int save_lastValidRow = screen->lastValidRow;

    const Cardinal noClick = 0;
    int save_numberOfClicks = screen->numberOfClicks;

    SelectUnit saveUnits = screen->selectUnit;
    SelectUnit saveMap = screen->selectMap[noClick];
#if OPT_SELECT_REGEX
    char *saveExpr = screen->selectExpr[noClick];
#endif
    SelectedCells *scp = &(screen->selected_cells[PRIMARY_CODE]);
    SelectedCells save_selection = *scp;

    char *result = 0;

    TRACE(("getDataFromScreen %s\n", method));

    memset(scp, 0, sizeof(*scp));

    screen->numberOfClicks = 1;
    lookupSelectUnit(xw, noClick, method);
    screen->selectUnit = screen->selectMap[noClick];

    memset(start, 0, sizeof(*start));
    if (IsBtnEvent(event)) {
	XButtonEvent *btn_event = (XButtonEvent *) event;
	CELL cell;
	screen->firstValidRow = 0;
	screen->lastValidRow = screen->max_row;
	PointToCELL(screen, btn_event->y, btn_event->x, &cell);
	start->row = cell.row;
	start->col = cell.col;
	finish->row = cell.row;
	finish->col = screen->max_col;
    } else {
	start->row = screen->cur_row;
	start->col = screen->cur_col;
	finish->row = screen->cur_row;
	finish->col = screen->max_col;
    }

    ComputeSelect(xw, start, finish, False, False);
    SaltTextAway(xw,
		 TargetToSelection(screen, PRIMARY_NAME),
		 &(screen->startSel), &(screen->endSel));

    if (scp->data_limit && scp->data_buffer) {
	TRACE(("...getDataFromScreen selection-data %.*s\n",
	       (int) scp->data_limit,
	       scp->data_buffer));
	result = malloc(scp->data_limit + 1);
	if (result) {
	    memcpy(result, scp->data_buffer, scp->data_limit);
	    result[scp->data_limit] = 0;
	}
	free(scp->data_buffer);
	scp->data_limit = 0;
    }

    TRACE(("...getDataFromScreen restoring previous selection\n"));

    screen->startSel = save_startSel;
    screen->startRaw = save_startRaw;
    screen->endSel = save_finishSel;
    screen->endRaw = save_finishRaw;

    screen->firstValidRow = save_firstValidRow;
    screen->lastValidRow = save_lastValidRow;

    screen->numberOfClicks = save_numberOfClicks;
    screen->selectUnit = saveUnits;
    screen->selectMap[noClick] = saveMap;
#if OPT_SELECT_REGEX
    screen->selectExpr[noClick] = saveExpr;
#endif

    screen->selected_cells[0] = save_selection;

    TrackText(xw, &save_old_start, &save_old_end);

    TRACE(("...getDataFromScreen done\n"));
    return result;
}