DisownSelection(XtermWidget xw)
{
    TScreen *screen = TScreenOf(xw);
    Atom *atoms = screen->selection_atoms;
    Cardinal count = screen->selection_count;
    Cardinal i;

    TRACE(("DisownSelection count %d, start %d.%d, end %d.%d\n",
	   count,
	   screen->startH.row,
	   screen->startH.col,
	   screen->endH.row,
	   screen->endH.col));

    for (i = 0; i < count; i++) {
	int cutbuffer = CutBuffer(atoms[i]);
	if (cutbuffer < 0) {
	    XtDisownSelection((Widget) xw, atoms[i],
			      screen->selection_time);
	}
    }
    /*
     * If none of the callbacks via XtDisownSelection() reset highlighting
     * do it now.
     */
    if (ScrnHaveSelection(screen)) {
	/* save data which will be reset */
	CELL first = screen->startH;
	CELL last = screen->endH;

	ResetSelectionState(screen);
	ReHiliteText(xw, &first, &last);
    } else {
	ResetSelectionState(screen);
    }
}