LoseSelection(Widget w, Atom *selection)
{
    TScreen *screen;
    Atom *atomP;
    Cardinal i;

    XtermWidget xw;

    if ((xw = getXtermWidget(w)) == 0)
	return;

    screen = TScreenOf(xw);
    TRACE(("LoseSelection %s\n", TraceAtomName(screen->display, *selection)));

    for (i = 0, atomP = screen->selection_atoms;
	 i < screen->selection_count; i++, atomP++) {
	if (*selection == *atomP)
	    *atomP = (Atom) 0;
	if (CutBuffer(*atomP) >= 0) {
	    *atomP = (Atom) 0;
	}
    }

    for (i = screen->selection_count; i; i--) {
	if (screen->selection_atoms[i - 1] != 0)
	    break;
    }
    screen->selection_count = i;

    for (i = 0, atomP = screen->selection_atoms;
	 i < screen->selection_count; i++, atomP++) {
	if (*atomP == (Atom) 0) {
	    *atomP = screen->selection_atoms[--screen->selection_count];
	}
    }

    if (screen->selection_count == 0)
	UnHiliteText(xw);
}