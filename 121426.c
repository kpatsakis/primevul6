formatVideoAttrs(XtermWidget xw, char *buffer, CELL *cell)
{
    TScreen *screen = TScreenOf(xw);
    LineData *ld = GET_LINEDATA(screen, cell->row);

    *buffer = '\0';
    if (ld != 0 && cell->col < (int) ld->lineSize) {
	IAttr attribs = ld->attribs[cell->col];
	const char *delim = "";

	if (attribs & INVERSE) {
	    buffer += sprintf(buffer, "7");
	    delim = ";";
	}
	if (attribs & UNDERLINE) {
	    buffer += sprintf(buffer, "%s4", delim);
	    delim = ";";
	}
	if (attribs & BOLD) {
	    buffer += sprintf(buffer, "%s1", delim);
	    delim = ";";
	}
	if (attribs & BLINK) {
	    buffer += sprintf(buffer, "%s5", delim);
	    delim = ";";
	}
#if OPT_ISO_COLORS
	if (attribs & FG_COLOR) {
	    Pixel fg = extract_fg(xw, ld->color[cell->col], attribs);
	    if (fg < 8) {
		fg += 30;
	    } else if (fg < 16) {
		fg += 90;
	    } else {
		buffer += sprintf(buffer, "%s38;5", delim);
		delim = ";";
	    }
	    buffer += sprintf(buffer, "%s%lu", delim, fg);
	    delim = ";";
	}
	if (attribs & BG_COLOR) {
	    Pixel bg = extract_bg(xw, ld->color[cell->col], attribs);
	    if (bg < 8) {
		bg += 40;
	    } else if (bg < 16) {
		bg += 100;
	    } else {
		buffer += sprintf(buffer, "%s48;5", delim);
		delim = ";";
	    }
	    (void) sprintf(buffer, "%s%lu", delim, bg);
	}
#endif
    }
}