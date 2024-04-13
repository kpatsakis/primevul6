ComputeSelect(XtermWidget xw,
	      CELL *startc,
	      CELL *endc,
	      Bool extend,
	      Bool normal)
{
    TScreen *screen = TScreenOf(xw);

    int cclass;
    CELL first = *startc;
    CELL last = *endc;
    Boolean ignored = False;

    struct {
	LineData *startSel;
	LineData *endSel;
    } ld;
    LineData *ltmp;

    TRACE(("ComputeSelect(startRow=%d, startCol=%d, endRow=%d, endCol=%d, %sextend)\n",
	   first.row, first.col,
	   last.row, last.col,
	   extend ? "" : "no"));

#if OPT_WIDE_CHARS
    if (first.col > 1
	&& isWideCell(first.row, first.col - 1)
	&& XTERM_CELL(first.row, first.col - 0) == HIDDEN_CHAR) {
	TRACE(("Adjusting start. Changing downwards from %i.\n", first.col));
	first.col -= 1;
	if (last.col == (first.col + 1))
	    last.col--;
    }

    if (last.col > 1
	&& isWideCell(last.row, last.col - 1)
	&& XTERM_CELL(last.row, last.col) == HIDDEN_CHAR) {
	last.col += 1;
    }
#endif

    if (Coordinate(screen, &first) <= Coordinate(screen, &last)) {
	screen->startSel = screen->startRaw = first;
	screen->endSel = screen->endRaw = last;
    } else {			/* Swap them */
	screen->startSel = screen->startRaw = last;
	screen->endSel = screen->endRaw = first;
    }

    InitRow(startSel);
    InitRow(endSel);

    switch (screen->selectUnit) {
    case Select_CHAR:
	(void) okPosition(screen, &(ld.startSel), &(screen->startSel));
	(void) okPosition(screen, &(ld.endSel), &(screen->endSel));
	break;

    case Select_WORD:
	TRACE(("Select_WORD\n"));
	if (okPosition(screen, &(ld.startSel), &(screen->startSel))) {
	    CELL mark;
	    cclass = CClassOf(startSel);
	    TRACE(("...starting with class %d\n", cclass));
	    do {
		mark = screen->startSel;
		--screen->startSel.col;
		if (screen->startSel.col < 0
		    && isPrevWrapped(startSel)) {
		    PrevRow(startSel);
		    screen->startSel.col = LastTextCol(screen, ld.startSel, screen->startSel.row);
		}
	    } while (screen->startSel.col >= 0
		     && CClassSelects(startSel, cclass));
	    if (normal)
		++screen->startSel.col;
	    else
		screen->startSel = mark;
	}
#if OPT_WIDE_CHARS
#define SkipHiddenCell(mark) \
	if (mark.col && XTERM_CELL(mark.row, mark.col) == HIDDEN_CHAR) \
	    mark.col++
#else
#define SkipHiddenCell(mark)	/* nothing */
#endif
	SkipHiddenCell(screen->startSel);

	if (!normal) {
	    screen->endSel = screen->startSel;
	    ld.endSel = ld.startSel;
	}

	if (okPosition(screen, &(ld.endSel), &(screen->endSel))) {
	    int length = LastTextCol(screen, ld.endSel, screen->endSel.row);
	    cclass = CClassOf(endSel);
	    TRACE(("...ending with class %d\n", cclass));
	    do {
		++screen->endSel.col;
		if (screen->endSel.col > length
		    && LineTstWrapped(ld.endSel)) {
		    if (!MoreRows(endSel))
			break;
		    screen->endSel.col = 0;
		    NextRow(endSel);
		    length = LastTextCol(screen, ld.endSel, screen->endSel.row);
		}
	    } while (screen->endSel.col <= length
		     && CClassSelects(endSel, cclass));
	    if (normal
		&& screen->endSel.col > length + 1
		&& MoreRows(endSel)) {
		screen->endSel.col = 0;
		NextRow(endSel);
	    }
	}
	SkipHiddenCell(screen->endSel);

	screen->saveStartW = screen->startSel;
	break;

    case Select_LINE:
	TRACE(("Select_LINE\n"));
	while (LineTstWrapped(ld.endSel)
	       && MoreRows(endSel)) {
	    NextRow(endSel);
	}
	if (screen->cutToBeginningOfLine
	    || screen->startSel.row < screen->saveStartW.row) {
	    screen->startSel.col = 0;
	    while (isPrevWrapped(startSel)) {
		PrevRow(startSel);
	    }
	} else if (!extend) {
	    if ((first.row < screen->saveStartW.row)
		|| (isSameRow(&first, &(screen->saveStartW))
		    && first.col < screen->saveStartW.col)) {
		screen->startSel.col = 0;
		while (isPrevWrapped(startSel)) {
		    PrevRow(startSel);
		}
	    } else {
		screen->startSel = screen->saveStartW;
	    }
	}
	trimLastLine(screen, &(ld.endSel), &(screen->endSel));
	break;

    case Select_GROUP:		/* paragraph */
	TRACE(("Select_GROUP\n"));
	if (okPosition(screen, &(ld.startSel), &(screen->startSel))) {
	    /* scan backward for beginning of group */
	    while (screen->startSel.row > 0 &&
		   (LastTextCol(screen, ld.startSel, screen->startSel.row -
				1) > 0 ||
		    isPrevWrapped(startSel))) {
		PrevRow(startSel);
	    }
	    screen->startSel.col = 0;
	    /* scan forward for end of group */
	    while (MoreRows(endSel) &&
		   (LastTextCol(screen, ld.endSel, screen->endSel.row + 1) >
		    0 ||
		    LineTstWrapped(ld.endSel))) {
		NextRow(endSel);
	    }
	    trimLastLine(screen, &(ld.endSel), &(screen->endSel));
	}
	break;

    case Select_PAGE:		/* everything one can see */
	TRACE(("Select_PAGE\n"));
	screen->startSel.row = 0;
	screen->startSel.col = 0;
	screen->endSel.row = MaxRows(screen);
	screen->endSel.col = 0;
	break;

    case Select_ALL:		/* counts scrollback if in normal screen */
	TRACE(("Select_ALL\n"));
	screen->startSel.row = -screen->savedlines;
	screen->startSel.col = 0;
	screen->endSel.row = MaxRows(screen);
	screen->endSel.col = 0;
	break;

#if OPT_SELECT_REGEX
    case Select_REGEX:
	do_select_regex(screen, &(screen->startSel), &(screen->endSel));
	break;
#endif

    case NSELECTUNITS:		/* always ignore */
	ignored = True;
	break;
    }

    if (!ignored) {
	/* check boundaries */
	ScrollSelection(screen, 0, False);
	TrackText(xw, &(screen->startSel), &(screen->endSel));
    }

    return;
}