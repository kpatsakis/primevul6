okSendFocusPos(XtermWidget xw)
{
    TScreen *screen = TScreenOf(xw);
    int result = screen->send_focus_pos;

    if (!AllowMouseOps(xw, emFocusEvent)) {
	result = False;
    }
    return result;
}