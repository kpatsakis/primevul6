SendMousePosition(XtermWidget xw, XEvent *event)
{
    XButtonEvent *my_event = (XButtonEvent *) event;
    Bool result = False;

    switch (okSendMousePos(xw)) {
    case MOUSE_OFF:
	/* If send_mouse_pos mode isn't on, we shouldn't be here */
	break;

    case BTN_EVENT_MOUSE:
    case ANY_EVENT_MOUSE:
	if (!OverrideEvent(event)) {
	    /* xterm extension for motion reporting. June 1998 */
	    /* EditorButton() will distinguish between the modes */
	    switch (event->type) {
	    case MotionNotify:
		my_event->button = 0;
		/* FALLTHRU */
	    case ButtonPress:
		/* FALLTHRU */
	    case ButtonRelease:
		EditorButton(xw, my_event);
		result = True;
		break;
	    }
	}
	break;

    case X10_MOUSE:		/* X10 compatibility sequences */
	if (IsBtnEvent(event)) {
	    if (!OverrideButton(my_event)) {
		if (my_event->type == ButtonPress)
		    EditorButton(xw, my_event);
		result = True;
	    }
	}
	break;

    case VT200_HIGHLIGHT_MOUSE:	/* DEC vt200 hilite tracking */
	if (IsBtnEvent(event)) {
	    if (!OverrideButton(my_event)) {
		if (my_event->type == ButtonPress &&
		    my_event->button == Button1) {
		    TrackDown(xw, my_event);
		} else {
		    EditorButton(xw, my_event);
		}
		result = True;
	    }
	}
	break;

    case VT200_MOUSE:		/* DEC vt200 compatible */
	if (IsBtnEvent(event)) {
	    if (!OverrideButton(my_event)) {
		EditorButton(xw, my_event);
		result = True;
	    }
	}
	break;

    case DEC_LOCATOR:
#if OPT_DEC_LOCATOR
	if (IsBtnEvent(event) || event->type == MotionNotify) {
	    result = SendLocatorPosition(xw, my_event);
	}
#endif /* OPT_DEC_LOCATOR */
	break;
    }
    return result;
}