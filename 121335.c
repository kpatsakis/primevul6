reallyInsertFormatted(Widget w, char *format, char *data, CELL *start, CELL *finish)
{
    XtermWidget xw;

    if ((xw = getXtermWidget(w)) != 0) {
	char *exps;

	if ((exps = expandFormat(xw, format, data, start, finish)) != 0) {
	    unparseputs(xw, exps);
	    unparse_end(xw);
	    free(exps);
	}
    }
}