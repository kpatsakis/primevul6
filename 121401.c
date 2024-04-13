FinishPaste64(XtermWidget xw)
{
    TScreen *screen = TScreenOf(xw);

    TRACE(("FinishPaste64(%d)\n", screen->base64_paste));
    if (screen->base64_paste) {
	screen->base64_paste = 0;
	unparseputc1(xw, screen->base64_final);
	unparse_end(xw);
    }
}