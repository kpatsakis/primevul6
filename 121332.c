okSendMousePos(XtermWidget xw)
{
    TScreen *screen = TScreenOf(xw);
    XtermMouseModes result = (XtermMouseModes) screen->send_mouse_pos;

    switch ((int) result) {
    case MOUSE_OFF:
	break;
    case X10_MOUSE:
	if (!AllowMouseOps(xw, emX10))
	    result = MOUSE_OFF;
	break;
    case VT200_MOUSE:
	if (!AllowMouseOps(xw, emVT200Click))
	    result = MOUSE_OFF;
	break;
    case VT200_HIGHLIGHT_MOUSE:
	if (!AllowMouseOps(xw, emVT200Hilite))
	    result = MOUSE_OFF;
	break;
    case BTN_EVENT_MOUSE:
	if (!AllowMouseOps(xw, emAnyButton))
	    result = MOUSE_OFF;
	break;
    case ANY_EVENT_MOUSE:
	if (!AllowMouseOps(xw, emAnyEvent))
	    result = MOUSE_OFF;
	break;
    case DEC_LOCATOR:
	if (!AllowMouseOps(xw, emLocator))
	    result = MOUSE_OFF;
	break;
    }
    return result;
}