UnhiliteSelection(XtermWidget xw)
{
    TScreen *screen = TScreenOf(xw);

    if (ScrnHaveSelection(screen)) {
	CELL first = screen->startH;
	CELL last = screen->endH;

	screen->startH = zeroCELL;
	screen->endH = zeroCELL;
	ReHiliteText(xw, &first, &last);
    }
}